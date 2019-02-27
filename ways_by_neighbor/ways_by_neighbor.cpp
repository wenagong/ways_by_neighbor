// ways_by_neighbor.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<pcl/io/pcd_io.h>
#include<fstream>
#include<pcl/registration/correspondence_estimation.h>
#include<pcl/features/normal_3d.h>
#include<pcl/kdtree/io.h>
#include<math.h>

using namespace std;

#define MAX_P 2 

int main()
{
	ofstream debug;
	debug.open("debug.txt");
	for (int p = 1; p < MAX_P; p++) {
		int q = p + 1;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_a(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_b(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_add(new pcl::PointCloud<pcl::PointXYZ>);
		char file_name1[30];
		sprintf_s(file_name1, "%s%d%s", "pcd_datas\\cloud",p,".pcd");
		char file_name2[30];
		sprintf_s(file_name2, "%s%d%s", "pcd_datas\\cloud",q,".pcd");
		pcl::io::loadPCDFile(file_name1, *cloud_a);
		pcl::io::loadPCDFile(file_name2, *cloud_b);
		*cloud_add = *cloud_a + *cloud_b;

		pcl::registration::CorrespondenceEstimation<pcl::PointXYZ, pcl::PointXYZ>core;
		core.setInputSource(cloud_a);
		core.setInputTarget(cloud_b);
		boost::shared_ptr<pcl::Correspondences>cor(new pcl::Correspondences);
		core.determineReciprocalCorrespondences(*cor, 0.12);

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

		pcl::io::savePCDFileASCII("overlapA.pcd", cloud_overlap1);
		pcl::io::savePCDFileASCII("overlapB.pcd", cloud_overlap2);

		pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal>ne1; //法线估计对象
		pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal>ne2;

		ne1.setInputCloud(cloud_overlap1.makeShared());
		ne1.setSearchSurface(cloud_add);

		ne2.setInputCloud(cloud_overlap2.makeShared());
		ne2.setSearchSurface(cloud_add);

		pcl::search::KdTree<pcl::PointXYZ>::Ptr kdtree1(new pcl::search::KdTree<pcl::PointXYZ>);
		pcl::search::KdTree<pcl::PointXYZ>::Ptr kdtree2(new pcl::search::KdTree<pcl::PointXYZ>);

		ne1.setSearchMethod(kdtree1);
		ne2.setSearchMethod(kdtree2);

		pcl::PointCloud<pcl::Normal>::Ptr nor_a(new pcl::PointCloud<pcl::Normal>);
		pcl::PointCloud<pcl::Normal>::Ptr nor_b(new pcl::PointCloud<pcl::Normal>);

		ne1.setRadiusSearch(0.8);
		ne2.setRadiusSearch(0.8);

		ne1.compute(*nor_a);
		ne2.compute(*nor_b);

		pcl::io::savePCDFileASCII("overlapA_normals.pcd", *nor_a);
		pcl::io::savePCDFileASCII("overlapB_normals.pcd", *nor_b);

		//p摄站下的中间法向量
		double angle_p = M_PI * (15 + (p - 1) * 30) / 180;
		double nor1_x = sin(angle_p);
		double nor1_z = cos(angle_p);

		//q摄站下的中间法向量
		double angle_q = M_PI * (15 + (q - 1) * 30) / 180;
		double nor2_x = sin(angle_q);
		double nor2_z = cos(angle_q);

		for (int j = 0; j < nor_a->size(); j++) {
			double nor_a_x = nor_a->points[j].normal_x;
			double nor_a_y = nor_a->points[j].normal_y;
			double nor_a_z = nor_a->points[j].normal_z;

			double nor_b_x = nor_b->points[j].normal_x;
			double nor_b_y = nor_b->points[j].normal_y;
			double nor_b_z = nor_b->points[j].normal_z;

			double res1 = (nor1_x * nor_a_x + nor1_z * nor_a_z) / (sqrt(nor1_x * nor1_x + nor1_z * nor2_z) * sqrt(nor_a_x * nor_a_x + nor_a_y * nor_a_y + nor_a_z * nor_a_z));
			double res2 = (nor2_x * nor_b_x + nor2_z * nor_b_z) / (sqrt(nor2_x * nor2_x + nor2_z * nor2_z) * sqrt(nor_b_x * nor_b_x + nor_b_y * nor_b_y + nor_b_z * nor_b_z));
			debug << "res1=" << res1 << " ; " << "res2=" << res2 << endl;
		}

	}
    return 0;
}
	
