#include "SharkSkeleton.h"


SharkBone* SharkSkeleton::gBone(string key)
{
	return bones.find(key)->second;
}

string SharkSkeleton::itoa(int key)
{
	char dec = (char) key%10;
	char tens = (char) (key/10)%10;
	char hunds = (char) (key/100)%10;
	char thous = (char) (key/1000)%10;
	char tents = (char) (key/10000)%10;
	return string(1,tents)+string(1,thous)+string(1,hunds)+string(1,tens)+string(1,dec);
}

SharkBone* SharkSkeleton::gBone(int key)  //bone getter where the key prefers to be an integer
{
	return gBone(itoa(key));
}

/*sets the bone, with its name as its key */
void SharkSkeleton::sBone(SharkBone* newBone)
{
	bones[newBone->gName()] = newBone;	
}

/*sets the angle on the armature. the key is the bone name. */
void SharkSkeleton::sAngle(string key, int angle)
{
	gBone(key)->changeAngle(angle);
}
void SharkSkeleton::sAngle(int key, int angle, bool downstream)
{
	gBone(itoa(key))->changeAngle(angle, downstream);
}
void SharkSkeleton::sAngle(string key, int angle, bool downstream)
{
	gBone(key)->changeAngle(angle, downstream);
}

void SharkSkeleton::sAngle(int key, int angle)
{
	sAngle(itoa(key),angle);
}
void SharkSkeleton::sAngle(string key, glQuaternion rotation)
{
	gBone(key)->changeAngle(rotation);
}

string SharkSkeleton::nextToken(char delimit, FILE* readFile)
{
	string numb;
	while(!feof(readFile))
	{
		char cur = fgetc(readFile);
		if(cur == delimit || cur == '\n') {break;}
		numb.append(1,cur);
	}
	return numb;
}

/*Builds an entire mesh and skeleton straight out of a .aobj file */
void SharkSkeleton::buildSkeletonAOBJ(string filename)
{
	FILE* readFile;
	readFile = fopen(filename.c_str(), "r");
     	if(readFile == NULL)
    	{
	   	printf(".aobj file not found\n");
	  	exit(-1);
 	} 

	nmesh->buildAOBJ(readFile);   
	//readFile has just read the first 'b' in the aobj file. The mesh is filled now

	

	//go though the rest of the parsing, pulling out each bone	
	char identifier = 'b';
	int index = 0;
	vector<vector<string> > boneRelationships = vector<vector<string> >();
	while(!feof(readFile))
	{
		//b name headRestArmature tailRestArmature ... child names ...
		if(identifier == 'b')
		{
			identifier = 0;
			char cur = fgetc(readFile);  //space
			vector<string> childNames = vector<string>();
			while(cur != '\n')  //per line
			{
				string bName = nextToken(' ', readFile);	
				Vector3f headr;
				headr.x = atof(nextToken(' ', readFile).c_str());
				headr.y = atof(nextToken(' ', readFile).c_str());
				headr.z = atof(nextToken(' ', readFile).c_str());

				Vector3f tailr;
				tailr.x = atof(nextToken(' ', readFile).c_str());
				tailr.y = atof(nextToken(' ', readFile).c_str());
				tailr.z = atof(nextToken(' ', readFile).c_str());
			
				SharkBone* nBone = new SharkBone(nmesh, index);
				nBone->buildBoneAOBJ(bName, headr, tailr);
				sBone(nBone);

				//child names need to be read.
				childNames.push_back(bName); //first name is the name of this bone
				fseek(readFile, -1, SEEK_CUR);
				cur = fgetc(readFile);
				while(cur != '\n')
				{
					childNames.push_back(nextToken(' ', readFile));
					fseek(readFile, -1, SEEK_CUR);
					cur = fgetc(readFile);
				}
				boneRelationships.push_back(childNames);
				index++;
			}
		}
		else
		{
			printf("problem reading aobj bones\n");
			exit(-1);
		}

		identifier = fgetc(readFile);
	}

	//Now, build the relationship tree among the bones
	for(int i = 0; i < boneRelationships.size(); i++)
	{
		for(int j = 1; j < boneRelationships[i].size(); j++)
		{
			gBone(boneRelationships[i][0])->addChild(gBone(boneRelationships[i][j]));	
		}	
	}
	bones[rootNode]->buildTranslation(bones[rootNode]->gHead(), Vector3f(0,0,0), Vector3f(0,0,0));

	fclose(readFile);


}


/* Builds the skeleton from the CalShark shark model. This is the non-general build method
 * Given the initial Blender mesh, build the smart mesh and the bones that make up the skeleton. 
 * Needs to know, the number of segments that make up the shark, and the length of the segments*/
void SharkSkeleton::buildSkeleton(Mesh* mesh, int numSegments, float *segLength)
{
	MyMat multiplier;  //this is here just to multiply matrices toghether
	float start; //start and end determine where to slice the mesh into bones. 
	float end;
	const int rootNum = 4;   //number down the spine where the root is.

	//Mesh transformation to smart sharkMesh form. 
	start = mesh->lengthMax; 
	end = mesh->lengthMax;

	rootNode = itoa(rootNum);   //set the name of the root
	//build the individual bones	
	for(int i = 0; i < numSegments; i++)
	{
		SharkBone * newBone = new SharkBone(nmesh, i);
		end = start;
		start -= segLength[i];
		newBone->buildBone(mesh, start, end, multiplier);
		if(i == rootNum-1){     //the translation ahead of the root node will not line up w/o an additional translation	
			MyMat forwardtrans;
			forwardtrans.makeTranslate(Vector3f(newBone->gLength(), 0, 0));
			newBone->sJointTranslation(forwardtrans);
			newBone->boneLengthToTranslation(i < rootNum);
		}
		else {
			newBone->boneLengthToTranslation(i < rootNum);
		}
		totalLength += newBone->gLength();
		newBone->sName(itoa(i));
		sBone(newBone); //name the bone and put it into the map.
	}

	//create the heirarchy in the bones
	for(int i = rootNum; i > 0; i--)  //from head to root
	{
		gBone(i)->addChild(gBone(i-1));   
	}
	for(int i = bones.size()-1; i > rootNum; i--)
	{
		gBone(i-1)->addChild(gBone(i));
	}
	armTranslation.makeTranslate(Vector3f(gBone(rootNode)->boneLength, 0, 0));
	nmesh->newUpdateApproved = true;
}

/*Applies matrix transformations to the SharkMesh. Accomplishes this by starting the recursive chain in transformHierarchy.
 * After calling this, the Keyframe is ready to save the vertex information. Flag in mesh (hasNewTransform) will be set. */
void SharkSkeleton::applyTransformation()
{
	//printf("vert %d quad %d \n", nmesh->vertSize(), nmesh->faceSize());  //sucessfully runs once and explodes
	nmesh->newUpdateApproved = false;
	//perform all the matrix magic in there to make a keyframe.

	MyMat stackMatrix = MyMat();

	//this sets the mesh back so it's aligned in world coordinates as it should be.
	stackMatrix.multRight(armTranslation);	

	nmesh->restPosition();   //reset all of the transformations
	bones[rootNode]->transformBone(&stackMatrix, !isLinearBlendSkinned);  //transform skeleton
	if(isLinearBlendSkinned){
		nmesh->linearBlendTransform();   //apply skeleton to skin
	}
	nmesh->hasNewTransform = true; //polling for new Keyframes will succeed now.
}

void SharkSkeleton::draw()
{
	MyMat stackMatrix = MyMat();

	//this sets the mesh back so it's aligned in world coordinates as it should be.
	stackMatrix.multRight(armTranslation);	

	bones[rootNode]->draw(&stackMatrix);
}

