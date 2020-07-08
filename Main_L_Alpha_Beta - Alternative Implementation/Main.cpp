//*** IMPLEMENTATION OF AN ENHANCED ADAPTATION
//*** OF THE REINHARD COLOUR TRANSFER METHOD
//***       L-alpha-beta version
//    See 'main' routine for further details.
//
// Copyright © Terry Johnson January April 2020
// Revised 07/07/2020 (Version 2)
// https://github.com/TJCoding

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>
#include <iostream>

int main(int argc, char *argv[]);
cv::Mat adjust_covariance(cv::Mat Lab[3], cv::Mat sLab[3],
                          float covLim);
cv::Mat convertTolab(cv::Mat input);
cv::Mat convertFromlab(cv::Mat input);


int main(int argc, char *argv[])
{
//  Transfers the colour distribution from the source image to the
//  target image by matching the mean and standard deviation and
//  the colour cross correlation in the L-alpha-beta colour space.

//	The implementation is an enhancement of a method described in
//  "Color Transfer between Images" paper by Reinhard et al., 2001,
//  but with additional processing options as follows.

//  Option 1
//  There is an option to match the cross correlation between
//  the colour channels 'alpha' and 'beta'.  Full matching, no
//  matching or restricted matching may be specified.
//  (See the note at the end of the code).

//  Option 2
//  There is an option to retain the shading of the target image
//  rather than adapt to the shading in the source image so
//  that the processing is implemented as a pure colour transfer.

//  Option 3
//  There is an option to iterate the processing more than once
//  (See the note at the end of the code).


// ##########################################################################
// #######################  PROCESSING SELECTIONS  ##########################
// ##########################################################################
    // Select the processing options in accordance with the
    // preceding descriptions.

    float CrossCovarianceLimit     = 0.5;  // Option 1 (Default is '0.5'.)
    bool KeepOriginalShading       = true; // Option 2 (Default is 'true'.)
    int  iterations                = 2;    // Option 3 (Default is '2'.)


    // Specify the image files that are to be processed,
    // where 'source image' provides the colour scheme that
    // is to be applied to 'target image'.

    std::string targetname = "images/Flowers_target.jpg";
    std::string sourcename = "images/Flowers_source.jpg";

// ###########################################################################
// ###########################################################################
// ###########################################################################

    // Declare variables
    cv::Scalar tmean, tdev, smean, sdev;

    // Read in the files.
    cv::Mat target = cv::imread(targetname, 1);
    cv::Mat source = cv::imread(sourcename, 1);

    cv::Mat targetf(target.size(),CV_32FC3);
    cv::Mat sourcef(source.size(),CV_32FC3);
    cv::Mat Lab[3], sLab[3];


    // Convert the source image from the BGR colour
    // space to the L-alpha-beta colour space.
    // Estimate the mean and standard deviation of
    // colour channels. Split the source image into
    // channels and standardise the data in the
    // colour channels (channels alpha and beta).
    // The standardised data has zero mean and
    // unit standard deviation.

    sourcef = convertTolab(source);

    cv::meanStdDev(sourcef, smean, sdev);
    cv::split(sourcef,sLab);
    sLab[1]=(sLab[1]-smean[1])/sdev[1];
    sLab[2]=(sLab[2]-smean[2])/sdev[2];

    for (int i=1;i<=iterations;i++)
    {
     // Condition the target data as previously
     // described for the source data.
     targetf=convertTolab(target);

     cv::meanStdDev(targetf, tmean, tdev);
     cv::split(targetf,Lab);
     Lab[1]=(Lab[1]-tmean[1])/tdev[1];
     Lab[2]=(Lab[2]-tmean[2])/tdev[2];

    // Implement cross covariance processing.
    // (no effect if CrossCovarianceLimit=0)
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
        Lab[0]=Lab[0]*sdev[0]+smean[0];
     }

     cv::merge(Lab,3,targetf);

     // Convert the processed image back to BGR values.
     target = convertFromlab(targetf);
    }

     // Display and save the final image.
     cv::imshow("processed image",target);
     cv::imwrite("images/processed.jpg", target);

    // Display images until a key is pressed.
     cv::waitKey(0);
     return 0;
   }



cv::Mat adjust_covariance(cv::Mat Lab[3], cv::Mat sLab[3], float covLim)
{
        // This routine adjusts colour channels 2 and 3 of
        // the image within the L-alpha-beta colour space.

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

        // Limit the size of W2 if required
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



// ##########################################################################
// ##### IMPLEMENTATION OF L-ALPHA-BETA FORWARD AND INVERSE TRANSFORMS ######
// ##########################################################################
// Coding taken from https://github.com/ZZPot/Color-transfer
// Credit to 'ZZPot'.
// I take responsibility for any issues arising my adaptation.


// Define the transformation matrices for L-alpha-beta transformation.
cv::Mat RGB_to_LMS = (cv::Mat_<float>(3,3) <<	0.3811f, 0.5783f, 0.0402f,
										        0.1967f, 0.7244f, 0.0782f,
                                                0.0241f, 0.1288f, 0.8444f);
float i3 = 1/sqrt(3), i6 = 1/sqrt(6), i2 = 1/sqrt(2);
cv::Mat LMS_to_lab = (cv::Mat_<float>(3,3) <<	i3, i3, i3,
										        i6, i6, -2*i6,
                                                i2, -i2, 0);

cv::Mat convertTolab(cv::Mat input)
{
	cv::Mat img_RGB (input.size(),CV_8UC3);
	cv::Mat img_RGBf(input.size(),CV_32FC3);
    cv::Mat img_lms (input.size(),CV_32FC3);
    cv::Mat img_lab (input.size(),CV_32FC3);

	// Swap channel order (so that transformation
	// matrices can be used in their familiar form).
	// Then convert to float.
	cv::cvtColor(input, img_RGB, CV_BGR2RGB);
	img_RGB.convertTo(img_RGBf, CV_32FC3, 1.0/255.f);

	// Apply stage 1 transform.
	cv::transform(img_RGBf, img_lms, RGB_to_LMS);

	// Define smallest permitted value and implement it.
    float epsilon =0.07;
	cv::Scalar min_scalar(epsilon, epsilon, epsilon);
	cv::Mat min_mat = cv::Mat(input.size(), CV_32FC3, min_scalar);
	cv::max(img_lms, min_mat, img_lms); // just before log operation.

	// Compute log10(x)  as ln(x)/ln(10)
	cv::log(img_lms,img_lms);
	img_lms=img_lms/log(10);

    // Apply stage 2 transform.
	cv::transform(img_lms, img_lab, LMS_to_lab);

	return img_lab;
}

cv::Mat convertFromlab(cv::Mat input)
{
	cv::Mat img_lms (input.size(),  CV_32FC3);
	cv::Mat img_RGBf(input.size(),  CV_32FC3);
	cv::Mat img_RGB (input.size(),  CV_8UC3);
	cv::Mat img_BGR (input.size(),  CV_8UC3);
    cv::Mat temp    (LMS_to_lab.size(),CV_32FC1);

    // Apply inverse of stage 2 transformation.
	cv::invert(LMS_to_lab,temp);
	cv::transform(input, img_lms, temp);

	// Compute 10^x as (e^x)^(ln10)
    cv::exp(img_lms,img_lms);
    cv::pow(img_lms,(double)log(10.0),img_lms);

    // Apply inverse of stage 1 transformation.
	cv::invert(RGB_to_LMS,temp);
	cv::transform(img_lms, img_RGBf, temp);

	// Convert to integer format and revert
	// channel ordering to BGR.
	img_RGBf.convertTo(img_RGB, CV_8UC3, 255.f);
	cv::cvtColor(img_RGB, img_BGR, CV_RGB2BGR);

	return img_BGR;
}
// ##########################################################################
// ##########################################################################
// ##########################################################################


// Notes on Cross Correlation Matching.
// ====================================
// Cross correlation matching is performed by operations of the
// form.
// Channel_alpha = W1*Channel_alpha + W2*Channel_beta
// Channel_beta  = W1*Channel_beta  + W2*Channel_alpha
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
// The 'Reinhard Method' of colour transfer modifies the distribution
// of values in the L-alpha-beta colour space and the enhanced version
// more so.  In some cases, these modified values may map back to RGB
// values outside their permitted range and they are then clipped back.
// To allow a better alignment of the intended and the actual properties
// of the target image this, there is an option to iterate the processing.
// A target image may be processed to align its colours with the
// source image but that resultant might then be processed itself
// to achieve better colour alignment. Experience suggests that two
// iterations (inclusive of the initial processing) give a good outcome.
// If only one iteration is undertaken, the image colour may be less
// intense.  This may be aesthetically more pleasing sometimes but
// two iterations will normally give an image that better matches
// the source image colouration.

// Note that when iteration is performed, the limiting for CrossCovarianceLimit
// is relaxed progressively at each iteration.



