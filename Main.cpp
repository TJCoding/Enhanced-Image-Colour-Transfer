//*** IMPLEMENTATION OF AN ENHANCED ADAPTATION
//*** OF THE REINHARD COLOUR TRANSFER METHOD
//    See 'main' routine for further details.
//
// Copyright © Terry Johnson January 11/01/2020
// Revised 07/07/2020 (Version 2)
// https://github.com/TJCoding

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>
#include <iostream>

int main(int argc, char *argv[]);
cv::Mat adjust_covariance(cv::Mat Lab[3], cv::Mat sLab[3], float covLim);
cv::Mat Rescale(cv::Mat lab_image);

int main(int argc, char *argv[])
{
//  Transfers the colour distribution from the source image to
//  the target image by matching the mean and standard deviation
//  and the colour cross correlation in the L*a*b* colour space.

//	The implementation is an enhancement of a method described in
//  "Color Transfer between Images" paper by Reinhard et al., 2001,
//  but with additional processing options as follows.

//  Option 1
//  There is an option to match the cross correlation between
//  the colour channels 'a' and 'b'.  Full matching, no
//  matching or restricted matching may be specified.
//  (See the note at the end of the code).


//  Option 2
//  There is an option to retain the shading of the target image
//  rather than adapt to the shading in the source image so
//  that the processing is implemented as a pure colour transfer.

//  Option 3
//  There is an option to scale the data to the available data
//  range when saturation (clipping) would otherwise occur.

//  Option 4
//  There is an option to iterate the processing more than once
//  (See the note at the end of the code).


// ##########################################################################
// #######################  PROCESSING SELECTIONS  ##########################
// ##########################################################################
    // Select the processing options in accordance with the
    // preceding descriptions.


    float CrossCovarianceLimit    = 0.5;    // Option 1 (Default is '0.5'.)
    bool  KeepOriginalShading     = true;   // Option 2 (Default is 'true'.)
    bool  ScaleRatherThanClip     = true;   // Option 3 (Default is 'true'.)
    int   iterations              = 2;      // Option 4 (Default is '2'.)


    // Specify the image files that are to be processed,
    // where 'source image' provides the colour scheme that
    // is to be applied to 'target image'.

    std::string targetname = "images/Flowers_target.jpg";
    std::string sourcename = "images/Flowers_source.jpg";

// ###########################################################################
// ###########################################################################
// ###########################################################################

    // Declare variables
    cv::Mat targetf, sourcef, Lab[3], sLab[3];
    cv::Scalar tmean, tdev, smean, sdev;

    // Read in the files.
    cv::Mat target = cv::imread(targetname, 1);
    cv::Mat source = cv::imread(sourcename, 1);


    // Convert the images from integer to float
    target.convertTo(targetf,CV_32FC3,1/255.0);
    source.convertTo(sourcef,CV_32FC3,1/255.0);

    // Convert the source image from the BGR colour
    // space to the L*a*b colour space.
    // Estimate the mean and standard deviation of
    // colour channels. Split the source image into
    // channels and standardise the data in the
    // colour channels (channels a and b). The
    // standardised data has zero mean and unit
    // standard deviation.

    cv::cvtColor(sourcef, sourcef, CV_BGR2Lab);
    cv::meanStdDev(sourcef, smean, sdev);
    cv::split(sourcef,sLab);
    sLab[1]=(sLab[1]-smean[1])/sdev[1];
    sLab[2]=(sLab[2]-smean[2])/sdev[2];

    for (int i=1;i<=iterations;i++)
    {
     // Condition the target data as previously
     // described for the source data.
     cv::cvtColor(targetf, targetf, CV_BGR2Lab);
     cv::meanStdDev(targetf, tmean, tdev);
     cv::split(targetf,Lab);
     Lab[1]=(Lab[1]-tmean[1])/tdev[1];
     Lab[2]=(Lab[2]-tmean[2])/tdev[2];

    // Implement cross covariance processing.
    // (no effect for CrossCovarianceLimit=0)
        float covLim=CrossCovarianceLimit*i/iterations;
        targetf=adjust_covariance(Lab, sLab, covLim);
        cv::split(targetf,Lab);

     // Rescale the previously standardised colour channels
     // so that the means and standard deviations now match
     // those of the source image.
     Lab[1]=Lab[1]*sdev[1]+smean[1];
     Lab[2]=Lab[2]*sdev[2]+smean[2];

     // If the original shading is not to be retained, then
     // match the shading to that of the source image by
     // standardising the data in the 'lightness' channel
     // of the target image and then rescaling to match
     // the corresponding channel of the source image.
     if(!KeepOriginalShading)
     {
        Lab[0]=(Lab[0]-tmean[0])/tdev[0];
        Lab[0]= Lab[0]*sdev[0]+smean[0];
     }

     cv::merge(Lab,3,targetf);

     // The final image data will automatically be clipped to
     // the range 0 to 255 (image saturation) unless rescaling
     // is selected.
     if(ScaleRatherThanClip){targetf=Rescale(targetf);}

     // Convert the processed image back to BGR values.
     cv::cvtColor(targetf, targetf,CV_Lab2BGR);
    }
     // Convert to 8 bit format.
     targetf.convertTo(target,CV_8UC3,255.0);

     // Display and save the final image.
     cv::imshow("processed image",target);
     cv::imwrite("images/processed.jpg", target);

    // Display images until a key is pressed.
     cv::waitKey(0);
     return 0;
   }

cv::Mat adjust_covariance(cv::Mat Lab[3], cv::Mat sLab[3], float covLim)
{
        // This routine adjusts colour channels 2 and 3
        // of the image within the L*a*b colour space.

        // The channels each have zero mean and unit
        // standard deviation but their cross correlation
        // value will not normally be zero.
        //
        // The processing reduces the cross correlation between
        // the channels to zero but then reintroduces
        // correlation such that the new cross correlation
        // value matches that for the source image.
        //
        // Throughout these manipulations the mean channel values
        // are maintained at zero and the standard deviations are
        // maintained as unity.
        //
        // The manipulations are based upon the following relationship.
        //
        // Let z1 and z2 be two independent (zero correlation) variables
        // with zero means and unit standard deviations. It can be shown
        // that variables a1 and a2 have zero means, unit standard
        // deviations, and mutual cross correlation 'R' when:
        //
        // a1=sqrt((1+R)/2)*z1 + sqrt((1-R)/2)*z2
        // a2=sqrt((1+R)/2)*z1 - sqrt((1-R)/2)*z2
        //
        // The above relationships are applied inversely to derive
        // uncorrelated standardised colour channels variables from
        // the standardised but correlated input channels.
        //
        // The above relationships are then applied directly to obtain
        // standardised correlated colour channels with correlation
        // matched to that of the source image colour channels.
        //
        // Original processing method attributable to Dr T E Johnson Sept 2019.

        // Declare variables
        float tcrosscorr, scrosscorr;
        float W1, W2, norm;
        cv::Mat temp1;
        cv::Scalar smean, sdev, temp2;

        // Compute the correlation for the target image colour channels.
        // The correlation between the standardised variables (zero mean,
        // unit standard deviation) can be computed as the mean cross
        // product for the two channels.
        cv::multiply(Lab[1],Lab[2],temp1);
        temp2=cv::mean(temp1);
        tcrosscorr =temp2[0];

        std::cout<<tcrosscorr<<" =tcrosscorr \n";

        // Compute the correlation for the source image colour channels.
        cv::multiply(sLab[1],sLab[2],temp1);
        temp2=cv::mean(temp1);
        scrosscorr =temp2[0];
        std::cout<<scrosscorr<<" =scrosscorr \n";

        // Adjust the correlation between the standardised input
        // channel values.
        cv::Mat z1=Lab[1].clone();
        cv::Mat z2=Lab[2].clone();

        W1= 0.5*sqrt((1+scrosscorr)/(1+tcrosscorr))
           +0.5*sqrt((1-scrosscorr)/(1-tcrosscorr));
        W2= 0.5*sqrt((1+scrosscorr)/(1+tcrosscorr))
           -0.5*sqrt((1-scrosscorr)/(1-tcrosscorr));

        if(std::abs(W2)>covLim*std::abs(W1))
        {
            W2=copysign(covLim*W1,W2);
            norm=1.0/sqrt(W1*W1+W2*W2+2*W1*W2*tcrosscorr);
            W1=W1*norm;
            W2=W2*norm;
        }

        Lab[1]=W1*z1+W2*z2;
        Lab[2]=W1*z2+W2*z1;

        cv:merge(Lab,3,temp1);
        return temp1;
}

cv::Mat Rescale(cv::Mat lab_image)
{
// Rescales an image in L*a*b format to match
// the permitted range representation in OpenCV

    // Declare variables
    cv::Mat Lab[3];
    cv::Point minLoc, maxLoc;
    double minVal, maxVal, scale=0.0, maxDev;

    // Split the image into channels.
    split(lab_image,Lab);

    // The L*a*b format as implemented in OpenCV requires that
    // the channel values lie within particular ranges. In the
    // absence of any corrective action, values outside the range
    // will automatically be clipped to the range limits.  This
    // would be an arbitrary change to the data which alters its
    // properties such that the processing as previously applied
    // to the data no longer exactly achieves its intend outcome.
    // This routine implements an alternative (also arbitrary)
    // modification to the data in which the data in each channel
    // is scaled back towards the centre of the range in order to
    // prevent range overflow.
    //
    // Colour channels 'a' and 'b' are scaled jointly to ensure
    // consistency.


    // Express the maximum and minimum values of the 'a' colour channel
    // as fractions of the maximum and minimum permitted values
    // (which are 127 and -127).  Keep the largest fraction to date.
    // A computed fractional value greater than one indicates
    // a data value outside the permitted range
    cv::minMaxLoc(Lab[1],&minVal, &maxVal, &minLoc, &maxLoc );
    scale=std::max(0.0,maxVal/127);
    scale=std::max(scale,-minVal/127);

    // Process channel 'b' similarly to channel 'a'.
    cv::minMaxLoc(Lab[2],&minVal, &maxVal, &minLoc, &maxLoc );
    scale=std::max(scale, maxVal/127);
    scale=std::max(scale,-minVal/127);

    std::cout<<"   "<<   scale << " scale\n";

    // If the largest channel excursion exceeds its permitted
    // range, then scale both the image channels back to bring
    // them within range.
    if (scale>1.0) {Lab[1]=Lab[1]/scale; Lab[2]=Lab[2]/scale;}

    // Express the maximum and minimum values of the 'lightness'
    // channel as fractions of the permitted deviations.
    // (50 +/-50 for the range 0 to 100.) A computed fractional
    // value greater than one indicates a data value outside the
    // permitted range, in response to which the data is rescaled.
    cv::minMaxLoc(Lab[0],&minVal, &maxVal, &minLoc, &maxLoc );
    scale=std::max((maxVal-50)/50, -(minVal-50)/50);
    if(scale>1) {Lab[0]=(Lab[0]-50)/scale+50;}

    merge(Lab,3,lab_image);

    return lab_image;
}

// Notes on Cross Correlation Matching.
// ====================================
// Cross correlation matching is performed by operations of the
// form.
// Channel_a = W1*Channel_a + W2*Channel_b
// Channel_b = W1*Channel_b + W2*Channel_a
// as determined by the value of CrossCovarianceLimit.
//
// If CrossCovarianceLimit = 0, W2=0 and no cross correlation
// matching is performed.
// If CrossCovarianceLimit > 0, W2 is clipped if necessary so
// that it cannot lie outside the range
// -CrossCovarianceLimit*W1 to +CrossCovarianceLimit*W1.
// This mechanism may be used to guard against an overly large
// correction term.
// Typically CrossCovarianceLimit might be set to 0.5
//(for a maximum modification corresponding to 50%).


// Notes on Iteration.
// ===================
// The 'Reinhard Method' of color transfer modifies the distribution
// of values in the L*a*b color space and the enhanced version more so.
// In some cases, the modified values may extend outside the permitted
// ranges for 'l', 'a' and 'b' values in the OpenCV implementation.
// Furthermore even values that are in range do not always give
// rise to valid BGR values when taken in combination.  For this reason,
// even when the L*a*b values are scaled to be within range, they
// can still give an output image which is different from that intended.
// To overcome this, there is an option to iterate the processing. A target
// image may be processed to align its colours with the source image but
// that resultant might then be processed itself to achieve better
// colour alignment. Experience suggests that two iterations with
// image rescaling (rather than image clipping) gives good outcomes.
// If only one iteration is undertaken, the image colour may be less
// intense.  This may be aesthetically more pleasing sometimes but
// two iterations will normally give an image that better matches
// the source image colouration.

// Note that when iteration is performed, the limiting for CrossCovarianceLimit
// is relaxed progressively at each iteration.

