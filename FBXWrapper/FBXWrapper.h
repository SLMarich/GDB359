#pragma once

#ifdef FBXWRAPPER_EXPORTS  
#define FBXWRAPPER_API __declspec(dllexport)   
#else  
#define FBXWRAPPER_API __declspec(dllimport)   
#endif  

#include "stdafx.h"

namespace FBXWrapper {
	class TestFunction {
	public:
		static FBXWRAPPER_API double add(double a, double b);
	};

	class FBXWRAPPER_API FBXInterface {
	private:

		const char* lFilename;		//File name to import

		int numTabs = 0;		/* Tab character ("\t") counter */

		struct fbxJoint { FbxNode *node; int parent_index; };
		std::vector<fbxJoint> jointArray;

	public: struct joint { float global_xform[16]; int parent_index; };
	private: std::vector<joint> transformArray;

		int indiciesCount;
		std::vector<int> indicies;	//Indicies to polygon vertexes
	public: struct vector4f { float data[4]; };
	private: std::vector<vector4f> vertices;
		

	public:
		FBXInterface();
		~FBXInterface();
		int InitializeImporter(const char* filename);
		void CreateScene();
		void PrintTabs();
		void PrintAttribute(FbxNodeAttribute* pAttribute);
		void PrintNode(FbxNode* pNode);
		void Print();
		void GetMesh();
		void bindArrayHelper(FbxNode * node, int parentIndex);
		void FillBindArray();
		void FillTransformArray();
		void PrintTransformArray();
		std::vector<joint> GetTransformArray();
		int GetIndiciesCount();
		std::vector<int> GetIndicies();
		std::vector<vector4f> GetVerticies();
	};
}