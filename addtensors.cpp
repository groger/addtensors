// STL includes
#include <string>
#include <iostream>
#include <fstream> 
#include <vector>

// ITK includes
#include <itkDiffusionTensor3D.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkDTITubeSpatialObject.h>
#include <itkVector.h>
#include <itkInterpolateImageFunction.h>
#include <itkTensorLinearInterpolateImageFunction.h>
//#include <itkVectorLinearInterpolateImageFunction.h>

// VTK includes
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkLine.h>

#include "deformationfieldoperations.h"
#include "dtitypes.h"
#include "addtensorsCLP.h"

int main(int argc, char* argv[])
{
  PARSE_ARGS;

  const unsigned int DIM = 3;
  typedef itk::DiffusionTensor3D<double> TensorPixelType;
  typedef itk::Image<TensorPixelType, DIM> TensorImageType;
  typedef TensorImageType::SizeType ImageSizeType;
  typedef TensorImageType::SpacingType ImageSpacingType;

// Read fiber file 

  if(fiberFile == "")
    {
      std::cerr << "A fiber file has to be specified" << std::endl;
      return EXIT_FAILURE;
    }
 
  
  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  std::cout << "Reading " << fiberFile<< std::endl;
  reader->SetFileName(fiberFile.c_str());
  reader->Update();
 
  // Extract the polydata
  vtkSmartPointer<vtkPolyData> polydata =
  reader->GetOutput();
 

// Setup tensor file if available
  typedef itk::ImageFileReader <TensorImageType> TensorImageReader;
  typedef itk::TensorLinearInterpolateImageFunction <TensorImageType, double> TensorInterpolateType;
  typedef TensorInterpolateType::ContinuousIndexType ContinuousIndexType;
  ContinuousIndexType ci;
  TensorImageReader::Pointer tensorreader = NULL;
  TensorInterpolateType::Pointer tensorinterp = NULL;
  
  if(tensorVolume != "")
  {
    tensorreader = TensorImageReader::New();
    tensorinterp = TensorInterpolateType::New();
    
    tensorreader->SetFileName(tensorVolume.c_str());
    try
    {
      tensorreader->Update();
      tensorinterp->SetInputImage(tensorreader->GetOutput());
    }
    catch(itk::ExceptionObject exp)
    {
      std::cerr << exp << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
	std::cerr <<"DTI file has to be specified!"<<std::endl;
	return EXIT_FAILURE;
  }


   vtkIdType numPoints= polydata->GetNumberOfPoints();
   std::cout<<"points: "<<numPoints<<std::endl;
   vtkPoints* inPts = polydata->GetPoints();
   double fiberpointtemp[3];

   typedef itk::DTITubeSpatialObject<3> DTITubeType;
   typedef DTITubeType::TubePointType DTIPointType;
   typedef DTIPointType::PointType PointType;
   PointType fiberpoint;
   vtkDoubleArray *dbar = vtkDoubleArray::New();
   dbar->SetNumberOfTuples(numPoints);
   dbar->SetNumberOfComponents(9);
   double tuple[9];

// For point each fiber
   for(int i=0;i<numPoints;i++)
   {//std::cout<<"step 1"<<std::endl;
   	inPts->GetPoint( i, fiberpointtemp );
	fiberpoint[0] = fiberpointtemp[0];
	fiberpoint[1] = fiberpointtemp[1];
	fiberpoint[2] = fiberpointtemp[2];
        // Attribute tensor data
	tensorreader->GetOutput()->TransformPhysicalPointToContinuousIndex(fiberpoint,ci); std::cout<<"ci "<<std::endl;
        if( !tensorreader->GetOutput()->GetLargestPossibleRegion().IsInside( ci ) )
        {
          std::cerr << "Fiber is outside of dti image"<< std::endl ;
        }
        itk::DiffusionTensor3D<double> tensor(tensorinterp->EvaluateAtContinuousIndex(ci).GetDataPointer());
	
	tuple[0]=tensor[0];
	tuple[1]=tensor[1];
	tuple[2]=tensor[2];
	tuple[3]=tensor[1];
	tuple[4]=tensor[3];
	tuple[5]=tensor[4];
	tuple[6]=tensor[2];
	tuple[7]=tensor[4];
	tuple[8]=tensor[5];

	dbar->InsertTuple(i,tuple);
    }

    polydata->GetPointData()->SetTensors(dbar);

  if(fiberOutput != "")
  {

  std::cout<<std::endl;
  std::cout<<"Saving fibers...."<<std::endl;
  std::cout<<fiberOutput<<std::endl;
  vtkPolyDataWriter * fiberwriter = vtkPolyDataWriter::New();
  fiberwriter->SetFileName(fiberOutput.c_str());
  fiberwriter->SetInput(polydata);
  fiberwriter->Update();
  try
    {
    fiberwriter->Write();
    std::cout<<"Done!"<<std::endl;
    }
  catch(...)
    {
    std::cout << "Error while saving fiber file." << std::endl;
    throw;
    }
	}

  return EXIT_SUCCESS;
}    



