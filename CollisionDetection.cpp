#include "CollisionDetection.h"
#include <math.h>

bool AABB_collision_detection(float * aabb1,float * aabb2){ //here it is expected that aabb comes in the format {xmin,ymin,xmax,ymax}
	if((aabb2[0]<aabb1[0] && aabb2[2]>aabb1[0]) ||  (aabb2[0]<aabb1[2] && aabb2[2]>aabb1[2])){
		if((aabb2[1]<aabb1[1] && aabb2[3]>aabb1[1]) ||  (aabb2[1]<aabb1[3] && aabb2[3]>aabb1[3])){
			return true;
		}
	}
	return false;
}

collision_info AABB_seperating_axis_collision_detection(float * aabb1,float * aabb2){
	collision_info result;
	result.overlap=INFINITY;
	result.from=1;
	float overlap=aabb1[2]-aabb2[0];
	if(overlap<0){
		result.overlap=-1;
		return result;
	}
	else if(result.overlap>overlap){
		result.overlap=overlap;
		result.axis[0]=1;
		result.axis[1]=0;
	}
	overlap=aabb2[2]-aabb1[0];
	if(overlap<0){
		result.overlap=-1;
		return result;
	}
	else if(result.overlap>overlap){
		result.overlap=overlap;
		result.axis[0]=-1;
		result.axis[1]=0;
	}

	overlap=aabb1[3]-aabb2[1];
	if(overlap<0){
		result.overlap=-1;
		return result;
	}
	else if(result.overlap>overlap){
		result.overlap=overlap;
		result.axis[0]=0;
		result.axis[1]=1;
	}
	overlap=aabb2[3]-aabb1[1];
	if(overlap<0){
		result.overlap=-1;
		return result;
	}
	else if(result.overlap>overlap){
		result.overlap=overlap;
		result.axis[0]=0;
		result.axis[1]=-1;
	}
	
	return result;
}

collision_info seperating_axis_collision_detection(float * vertex1,float * axis1,int n_vertex1,float * vertex2,float * axis2,int n_vertex2){
	collision_info result;
	result.overlap=INFINITY;
	
	for(int k=0;k<n_vertex1;k++){
		float max1=-INFINITY,min2=INFINITY;

		for(int i=0;i<n_vertex1;i++){
			for(int j=0;j<n_vertex2;j++){
				float temp1=axis1[k*2]*vertex1[i*2]+axis1[k*2+1]*vertex1[i*2+1],temp2=axis1[k*2]*vertex2[j*2]+axis1[k*2+1]*vertex2[j*2+1];
				if(max1<temp1)
					max1=temp1;
				if(min2>temp2)
					min2=temp2;
			}
		}
		float overlap=max1-min2;
		if(overlap<0){ //the polygons are not overlapping on the axis and the operation is terminated with a -1 in overlap aka it is invalid
			result.overlap=-1;
			return result;
		}
		else if(overlap<result.overlap){
			result.overlap=overlap;
			result.axis[0]=axis1[k*2],result.axis[1]=axis1[k*2+1];
			result.from=1;
		}
	}

	for(int k=0;k<n_vertex2;k++){
		float max2=-INFINITY,min1=INFINITY;

		for(int i=0;i<n_vertex1;i++){
			for(int j=0;j<n_vertex2;j++){
				float temp1=axis2[k*2]*vertex1[i*2]+axis2[k*2+1]*vertex1[i*2+1],temp2=axis2[k*2]*vertex2[j*2]+axis2[k*2+1]*vertex2[j*2+1];
				if(max2<temp2)
					max2=temp2;
				if(min1>temp1)
					min1=temp1;
			}
		}
		float overlap=max2-min1;
		if(overlap<0){ //the polygons are not overlapping on the axis and the operation is terminated with a -1 in overlap aka it is invalid
			result.overlap=-1;
			return result;
		}
		else if(overlap<result.overlap){
			result.overlap=overlap;
			result.axis[0]=axis2[k*2],result.axis[1]=axis2[k*2+1];
			result.from=2;
		}
	}

	return result;
}


void calculate_axis(float * vertex,float * axis,int n_vertex){
	float prevx=vertex[n_vertex-2],prevy=vertex[n_vertex-1];
	for(int i=0;i<n_vertex;i++){
		float tempx=vertex[i*2]-prevx,tempy=vertex[2*i+1]-prevy;
		float l=sqrt(pow(tempx,2)+pow(tempy,2));
		axis[i*2]=-tempy/l,axis[i*2+1]=tempx/l;	
	}
}

void calculate_AABB(float * vertex,float * aabb,int n_vertex){
	aabb[0]=INFINITY;
	aabb[1]=INFINITY;
	aabb[2]=-INFINITY;
	aabb[3]=-INFINITY;

	for(int i=0;i<n_vertex;i++){
		float x=vertex[i*2],y=vertex[i*2+1];
		if(x<aabb[0])
			aabb[0]=x;
		if(x>aabb[2])
			aabb[2]=x;
		if(y<aabb[1])
			aabb[1]=y;
		if(y>aabb[3])
			aabb[3]=y;

	}
}


void is_clockwise(float * vertex,int n_vertex){
	float Area=0;

	for(int i=0;i<n_vertex-1;i++)
		Area+=vertex[2*i]*vertex[2*(i)+3]-vertex[2*i+1]*vertex[2*(i)+2];

	Area+=vertex[n_vertex-2]*vertex[1]-vertex[n_vertex-1]*vertex[0];

	Area*=0.5;

	if(Area<0){
		//needs to reverse vertexes
		for(int i=0;i<n_vertex/2;i++){
			float tempx=vertex[i*2],tempy=vertex[i*2+1];
			vertex[i*2]=vertex[n_vertex-i*2-2];
			vertex[i*2+1]=vertex[n_vertex-i*2-1];
			vertex[n_vertex-i*2-2]=tempx;
			vertex[n_vertex-i*2-1]=tempy;
		}
		Area*=-1;
	}
}
