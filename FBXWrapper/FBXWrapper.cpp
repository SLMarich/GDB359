// FBXWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FBXWrapper.h"

namespace FBXWrapper {
	FBXWRAPPER_API double TestFunction::add(double a, double b) {
		return a + b;
	}

	static FbxManager *lSdkManager;	//Manager pointer
	static FbxIOSettings *ioSettings;	//IO settings pointer
	static FbxImporter *lImporter;		//Importer pointer
	static FbxScene *lScene;			//Scene pointer
	static FbxNode *lRootNode;			//Pointer to root node of scene
	static FbxMesh *lMesh;				//Pointer to the mesh of the scene
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);

	FBXInterface::FBXInterface()
	{
		//Create manager
		lSdkManager = FbxManager::Create();
		//Create IO settings
		ioSettings = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ioSettings);

		//Create an importer
		lImporter = FbxImporter::Create(lSdkManager, "");
	}

	FBXInterface::~FBXInterface()
	{
		// Destroy the SDK manager and all the other objects it was handling.
		lSdkManager->Destroy();
	}

	int FBXInterface::InitializeImporter(const char * filename)
	{
		// Use the first argument as the filename for the importer.
		if (!lImporter->Initialize(filename, -1, lSdkManager->GetIOSettings())) {
			std::cout << "Call to FbxImporter::Initialize() failed.\n";
			std::cout << "Error returned: " << lImporter->GetStatus().GetErrorString() << "\n\n";
			exit(-1);
		}
		return 0;
	}

	void FBXInterface::CreateScene()
	{
		// Create a new scene so that it can be populated by the imported file.
		lScene = FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(lScene);

		// The file is imported, so get rid of the importer.
		lImporter->Destroy();
	}

	/**
	* Print the required number of tabs.
	*/
	void FBXInterface::PrintTabs() {
		for (int i = 0; i < numTabs; i++)
			std::cout<<"\t";
	}

	/**
	* Return a string-based representation based on the attribute type.
	*/
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
		switch (type) {
		case FbxNodeAttribute::eUnknown: return "unidentified";
		case FbxNodeAttribute::eNull: return "null";
		case FbxNodeAttribute::eMarker: return "marker";
		case FbxNodeAttribute::eSkeleton: return "skeleton";
		case FbxNodeAttribute::eMesh: return "mesh";
		case FbxNodeAttribute::eNurbs: return "nurbs";
		case FbxNodeAttribute::ePatch: return "patch";
		case FbxNodeAttribute::eCamera: return "camera";
		case FbxNodeAttribute::eCameraStereo: return "stereo";
		case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
		case FbxNodeAttribute::eLight: return "light";
		case FbxNodeAttribute::eOpticalReference: return "optical reference";
		case FbxNodeAttribute::eOpticalMarker: return "marker";
		case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary: return "boundary";
		case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case FbxNodeAttribute::eShape: return "shape";
		case FbxNodeAttribute::eLODGroup: return "lodgroup";
		case FbxNodeAttribute::eSubDiv: return "subdiv";
		default: return "unknown";
		}
	}

	/**
	* Print an attribute.
	*/
	void FBXInterface::PrintAttribute(FbxNodeAttribute* pAttribute) {
		if (!pAttribute) return;

		FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
		FbxString attrName = pAttribute->GetName();
		PrintTabs();
		// Note: to retrieve the character array of a FbxString, use its Buffer() method.
		std::cout<<"<attribute type='"<<typeName.Buffer()<<"' name='"<< attrName.Buffer() <<"'/>\n";
	}

	/**
	* Print a node, its attributes, and all its children recursively.
	*/
	void FBXInterface::PrintNode(FbxNode* pNode) {
		PrintTabs();
		const char* nodeName = pNode->GetName();
		FbxDouble3 translation = pNode->LclTranslation.Get();
		FbxDouble3 rotation = pNode->LclRotation.Get();
		FbxDouble3 scaling = pNode->LclScaling.Get();

		// Print the contents of the node.
		std::cout << "<node name='" << nodeName << "'" << std::endl;
		PrintTabs();
		std::cout << "translation = '(" << translation[0] << ", " << translation[1] << ", " << translation[2] << ")'" << std::endl;
		PrintTabs();
		std::cout << "rotation = '(" << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << ")'" << std::endl;
		PrintTabs();
		std::cout << "scaling = '(" << scaling[0] << ", " << scaling[1] << ", " << scaling[2] << ")' > \n";
		numTabs++;

		// Print the node's attributes.
		for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
			PrintAttribute(pNode->GetNodeAttributeByIndex(i));

		// Recursively print the children.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			PrintNode(pNode->GetChild(j));

		numTabs--;
		PrintTabs();
		std::cout << "</node>\n";
	}


	void FBXInterface::Print()
	{
		// Print the nodes of the scene and their attributes recursively.
		// Note that we are not printing the root node because it should
		// not contain any attributes.
		lRootNode = lScene->GetRootNode();
		std::cout << "Testing cout... lRootNode is at: " << lRootNode << std::endl;
		if (lRootNode) {
			for (int i = 0; i < lRootNode->GetChildCount(); i++)
				PrintNode(lRootNode->GetChild(i));
		}
	}

	void FBXInterface::GetMesh()
	{
		//Find the first bind pose
		int poseCount = lScene->GetPoseCount();
		FbxPose *currentPose;
		for (int i = 0; i < poseCount; i++) {
			currentPose = lScene->GetPose(i);
			if (currentPose->IsBindPose()) {
				//Find the skeleton root
				int nodeCount = currentPose->GetCount();
				FbxNode *currentNode;
				FbxMesh	*currentMesh;
				for (int j = 0; j < nodeCount; j++) {
					currentNode = currentPose->GetNode(i);
					currentMesh = currentNode->GetMesh();
					if (currentMesh) {
						indiciesCount = currentMesh->GetPolygonVertexCount();
						int polygonCount = currentMesh->GetPolygonCount(); //Get number of polygons
						for (int perPoly = 0; perPoly < polygonCount; perPoly++) {
							int vertsCount = currentMesh->GetPolygonSize(perPoly); //Get the number of verts in the poly
							for (int perVert = 0; perVert < vertsCount; perVert++) {
								int indexHolder = currentMesh->GetPolygonVertex(perPoly, perVert);
								indicies.push_back(currentMesh->GetPolygonVertex(perPoly, perVert));
							}
						}
						//indicies = currentMesh->GetPolygonVertices();
						int controlPointCount = currentMesh->GetControlPointsCount();
						for (int k = 0; k < controlPointCount; k++) {
							vector4f holder;
							holder.data[0] = (float)currentMesh->GetControlPoints()[k][0];
							holder.data[1] = (float)currentMesh->GetControlPoints()[k][1];
							holder.data[2] = (float)currentMesh->GetControlPoints()[k][2];
							holder.data[3] = (float)currentMesh->GetControlPoints()[k][3];
							vertices.push_back(holder);
						}
						break; //Break once you've found it
					}
				}
				break;
			}
		}
	}

	void FBXInterface::bindArrayHelper(FbxNode *node, int parentIndex) {
		fbxJoint jointHolder;
		jointHolder.node = node;
		jointHolder.parent_index = parentIndex;
		int newParentIndex = jointArray.size();
		jointArray.push_back(jointHolder);
		int childCount = node->GetChildCount();
		for (int i = 0; i < childCount; i++) {
			bindArrayHelper(node->GetChild(i), newParentIndex);
		}
	}
	
	void FBXInterface::FillBindArray()
	{
		//Find the first bind pose
		int poseCount = lScene->GetPoseCount();
		FbxPose *currentPose;
		for (int i = 0; i < poseCount; i++) {
			currentPose = lScene->GetPose(i);
			if (currentPose->IsBindPose()) {
				//Find the skeleton root
				int nodeCount = currentPose->GetCount();
				FbxNode *currentNode;
				FbxSkeleton	*currentSkeleton;
				for (int i = 0; i < nodeCount; i++) {
					currentNode = currentPose->GetNode(i);
					currentSkeleton = currentNode->GetSkeleton();
					if (currentSkeleton) {
						if (currentSkeleton->IsSkeletonRoot()) {
							//Fill out the vector
							bindArrayHelper(currentNode, -1);
							break; //Break once you've found it
						}
					}
				}
				break;
			}
		}
	}

	//Fills out the transform array using the bind array
	void FBXInterface::FillTransformArray() {
		joint holderJoint;
		FbxMatrix transformHolder;
		//Step through the joint array
		for (unsigned int i = 0; i < jointArray.size(); i++) {
			//Copy over the parent index
			holderJoint.parent_index = jointArray.at(i).parent_index;
			//Get the transformation matrix of the joint
			transformHolder = jointArray.at(i).node->EvaluateGlobalTransform();
			//Copy it over
			//std::cout << "\nJoint Array Before Fill:"<<std::endl;
			for (int vec = 0; vec < 4; vec++) {
				for (int doubleNumber = 0; doubleNumber < 4; doubleNumber++) {
					holderJoint.global_xform[vec+doubleNumber] = (float)transformHolder.mData[vec].mData[doubleNumber];
					//std::cout << transformHolder.mData[vec].mData[doubleNumber]<<" ";
				}
				//std::cout << "\t";
			}
			//std::cout << "\n";
			//Add the completed joint to the transform array
			transformArray.push_back(holderJoint);
		}
	}

	void FBXInterface::PrintTransformArray() {
		for (unsigned int currentJoint = 0; currentJoint < transformArray.size(); currentJoint++) {
			std::cout << "Joint: " << "\t";
			std::cout << "Parent Index: " << transformArray.at(currentJoint).parent_index <<"\t";
			std::cout << "Transform Array:" << std::endl;
			for (int i = 0; i < 4; i++) {
				for (int h = 0; h < 4; h++) {
					std::cout << transformArray.at(currentJoint).global_xform[i+h] << " ";
				}
				std::cout<<"\n";
			}
			std::cout << std::endl;
		}
	}
	std::vector<FBXInterface::joint> FBXInterface::GetTransformArray()
	{
		return transformArray;
	}
	int FBXInterface::GetIndiciesCount()
	{
		return indiciesCount;
	}
	std::vector<int> FBXInterface::GetIndicies()
	{
		return indicies;
	}
	std::vector<FBXInterface::vector4f> FBXInterface::GetVerticies()
	{
		return vertices;
	}
}

