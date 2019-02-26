// ways_by_neighbor.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<pcl/io/pcd_io.h>
#include<fstream>
#include<pcl/registration/correspondence_estimation.h>
using namespace std;

#define MAX_P 13 

int main()
{
	for (int p = 1; p < MAX_P; p++) {
		int q = p + 1;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_a(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_b(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_add(new pcl::PointCloud<pcl::PointXYZ>);
		char file_name1[20];
		sprintf_s(file_name1, "%s%d%s", "pcd_datas\\",p,".pcd");
		char file_name2[20];
		sprintf_s(file_name2, "%s%d%s", "pcd_datas\\",q, ".pcd");
		pcl::io::loadPCDFile(file_name1, *cloud_a);
		pcl::io::loadPCDFile(file_name2, *cloud_b);
		*cloud_add = *cloud_a + *cloud_b;

		pcl::registration::CorrespondenceEstimation<pcl::PointXYZ, pcl::PointXYZ>core;
		core.setInputSource(cloud_a);
		core.setInputTarget(cloud_b);
		boost::shared_ptr<pcl::Correspondences>cor(new pcl::Correspondences);
		core.determineReciprocalCorrespondences(*cor, 0.01);

		pcl::PointCloud<pcl::PointXYZ>cloud_overlap1;
		cloud_overlap1.width = cor->size();
		cloud_overlap1.height = 1;
		cloud_overlap1.is_dense = false;
		cloud_overlap1.resize(cloud_overlap1.width*cloud_overlap1.height);

		pcl::PointCloud<pcl::PointXYZ>cloud_overlap2;
		cloud_overlap2.width = cor->size();
		cloud_overlap2.height = 1;
		cloud_overlap2.is_dense = false;
		cloud_overlap2.resize(cloud_overlap2.width*cloud_overlap2.height);

		for (int i = 0; i < cor->size(); i++) {
			cloud_overlap1.points[i].x = cloud_a->points[cor->at(i).index_query].x;
			cloud_overlap1.points[i].y = cloud_a->points[cor->at(i).index_query].y;
			cloud_overlap1.points[i].z = cloud_a->points[cor->at(i).index_query].z;

			cloud_overlap2.points[i].x = cloud_b->points[cor->at(i).index_match].x;
			cloud_overlap2.points[i].y = cloud_b->points[cor->at(i).index_match].y;
			cloud_overlap2.points[i].z = cloud_b->points[cor->at(i).index_match].z;
		}

		for (int j = 0; j < cloud_overlap1.size(); j++) {
			
		}

	}
    return 0;
}

