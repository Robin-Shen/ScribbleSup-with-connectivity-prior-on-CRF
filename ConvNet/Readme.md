
# A Deep Convolutional Network for Semantic Segmentation  
This code provides an implementation of the DeepLab Semantic Segmentation Network (https://arxiv.org/abs/1606.00915).  
  
It uses the Code of zhengyang-wang as a basis: https://github.com/zhengyang-wang/Deeplab-v2--ResNet-101--Tensorflow  
**The original Readme with descriptions how to run training, testing and validation can be found in the directory *segmentation-local*.**  
There are 2 versions of the Code:  
* "segmentation-floyd" contains the code modified to run on Floydhub.  
* "segmentation-local" contains the code to run locally and has been modified to output not the predicted class, but the probabilites of the classes for given superpixels.  
  
## Superpixel Data
To get the predictions for the classes for each superpixel, the predictor needs a superpixel.zip-file in the parent folder containing .txt files with the assignment to the superpixels for each pixel. As a example, how the files should be structured, an example superpixel.zip file with a few example files is been provided.
  
## Dependencies:  
* Tensorflow (https://www.tensorflow.org)  
  
## Additional Files:  
the parent folder of "segmentation-local" has 2 folders which have to be filled with data.
Because of the size of the data, there are just Download links provided:  
| Folder | Description | Download |  
| ------ | ------ | ------  |  
| reference model | The reference models to start training, "deeplab_resnet_init.ckpt" | https://drive.google.com/drive/folders/0B_rootXHuswsZ0E4Mjh1ZU5xZVU |  
| VOCdevkit | Training and test images from the PASCAL 2012 Dataset | http://host.robots.ox.ac.uk/pascal/VOC/ |  
For the run on Floydhub, the both folders have to be uploaded independently from the code as additional data files in a folder called "data".  
**For modifications of the paths, open and edit the "main.py" files in the respective folders.**



