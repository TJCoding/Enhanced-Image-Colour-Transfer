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
_Initial indications are that the L\*a\*b implementation outperforms the L-alpha-beta implementation on image pairs that are of good quality and well matched but L-alpha-beta may show greater resilience in other cases.  Feedback would be welcome.  Of particular interest would be examples which highlight the differences between standard processing, the two versions of enhanced processing and Photoshop processing._