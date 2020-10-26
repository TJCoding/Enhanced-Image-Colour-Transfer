# Enhanced Image-Colour-Transfer
============================================================

##### An Enhanced Implementation of the Colour Transfer Method proposed by E Reinhard et al.

A revised version of the existing method is proposed which utilises image representation in the L\*a\*b colour space.  As with previous implementations, the method takes account of the mean and standard deviation values of the respective L\*a\*b components but, in addition, it addresses the correlation between the 'a' and 'b' colour components.  This method is described further in the file '[Enhanced Image-Colour-Transfer.pdf](Documents/Enhanced%20Image-Colour-Transfer.pdf)' in the Documents folder.

Additional options allow for:- the rescaling of data to constrain it within range; recursive iteration of the transfer process, and a choice of shading method.

The program 'Main.cpp' runs under C++ using OpenCV.  Processing options may be specified by modifying statements within the 'Processing Selections' section in the main routine.

The examples shown below have been selected to illustrate the differences between the different processing methods.  For other image combinations, the differences may be less noticeable.
#  
#  
*The images below are as follows: - target image, source image, image after basic processing and image after enhanced processing.   For basic processing, there is a single iteration with data clipping, no cross correlation processing and source-image-derived shading.   For enhanced processing, there are two iterations, with data scaling, cross correlation processing and target-image-derived shading.*   

![Composite of Corn Image: Inputs and Outputs](Documents/Images/Corn_composite.jpg?raw=true)

![Composite of Ocean Image: Inputs and Outputs](Documents/Images/Ocean_composite.jpg?raw=true)

![Composite of Flowers Image: Inputs and Outputs](Documents/Images/Flowers_composite.jpg?raw=true)
#   
#   
*The images below are as follows:- image after enhanced processing, image after processing by a well-known commercial software application, image after processing by a new proprietary method developed by the author, image after processing by an on line facility which uses a neural algorithm of artistic style.*  

![Second Composite of Flowers Image: Inputs and Outputs](Documents/Images/Flowers2_composite.jpg?raw=true)
#      
#      
*A second implementation of enhanced processing has been provided in the folder 'Main_L_Alpha_Beta'. This utilises an alternative colour space representation developed by Ruderman et al.  This colour space was integral to the original colour transfer method as proposed by Reinhard et al.*
#     
_Initial indications are that, for many image pairs, the L\*a\*b and the L-alpha-beta implementations achieve similar quality images, even though there may be slight differences.  However, some instances have been found where the L\*a\*b implementation produces a poor quality image but the L-alpha-beta implementation does not. This is further discussed in the file '[Enhanced Image-Colour- Transfer --- LAlphaBeta](Documents/Enhanced%20Image-Colour-%20Transfer%20---%20LAlphaBeta.pdf)' in the Documents folder.  Feedback would be welcome.  Of particular interest would be examples which highlight the differences between standard processing, the two versions of enhanced processing and Photoshop processing._

A new implementation with improved processing options is now provided in the _'Further Enhanced Processing'_ folder, and is described below. This is based upon L-alpha-beta processing and is now recommended in preference to the processing described above.  
#  
# 
============================================================================ 
# Further Enhanced Image-Colour-Transfer
============================================================================

##### A Further Enhanced Implementation of the Colour Transfer Method proposed by E Reinhard et al.

A further revised version of the standard basic processing method is proposed which utilises image representation in the l-alpha-beta  colour space and which supersedes the Enhanced Method described above. As with the basic processing, account is taken of the the mean and standard deviation values of the respective l-alpha-beta components but, in addition, the correlation between the 'alpha' and 'beta' colour components is adddressed. Additional new options allow for the matching of higher moments of the colour components beyond the second moments, for the adjustment of image saturation and for the implementation of flexible image shading options.  The processing is described further in the file '[Further Enhanced Image-Colour-Transfer.pdf ](Further%20Enhanced%20Processing/Documents/Further%20Enhanced%20Image-Colour-Transfer.pdf)' in the 'Documents' sub-folder of the 'Further Enhanced Processing' folder . 

The program '[Main.cpp ](Further%20Enhanced%20Processing/Main.cpp)' in the 'Further Enhanced Processing' folder runs under C++ using OpenCV. Processing options may be specified by modifying statements within the 'Processing Selections' section in the main routine.

The following images illustrate the processing describe here.

*The first image in each set is the target image and the second is the colour source image.  The third image shows the effects of standard basic processing as proposed by E Reinhard et al. The final image has been subject to the further enhanced processing using the default processing options specified in the source code and in its asssociated documentation.* 

![Composite of Vase Image: Inputs and Outputs](Documents/Images/Vase_composite.jpg?raw=true)

![Composite of Autumn Image: Inputs and Outputs](Documents/Images/Autumn_composite.jpg?raw=true)

![Composite of Flowers Image: Inputs and Outputs](Documents/Images/Flowers3_composite.jpg?raw=true)


