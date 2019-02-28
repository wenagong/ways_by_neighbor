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

#define MAX_P 13

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
		sprintf_s(file_name1, "%s%d%s", "result_datas\\cloud",p,".pcd");

		//避免重复去冗
		if (pcl::io::loadPCDFile<pcl::PointXYZ>(file_name1, *cloud_a) == -1) {  //在结果目录查找失败的话，返回初始目录

			char init_name1[30];
			sprintf_s(init_name1, "%s%d%s", "pcd_datas\\cloud", p, ".pcd");  //初始pcd文件目录

			pcl::io::loadPCDFile<pcl::PointXYZ>(init_name1, *cloud_a);
		}

		char file_name2[30];
		sprintf_s(file_name2, "%s%d%s", "result_datas\\cloud", q, ".pcd");

		if (pcl::io::loadPCDFile<pcl::PointXYZ>(file_name2, *cloud_b) == -1) {  //读取失败

			char init_name2[30];
			sprintf_s(init_name2, "%s%d%s", "pcd_datas\\cloud", q, ".pcd");  //初始pcd文件目录

			pcl::io::loadPCDFile<pcl::PointXYZ>(init_name2, *cloud_b);
		}

		*cloud_add = *cloud_a + *cloud_b;

		pcl::registration::CorrespondenceEstimation<pcl::PointXYZ, pcl::PointXYZ>core;
		core.setInputSource(cloud_a);
		core.setInputTarget(cloud_b);
		boost::shared_ptr<pcl::Correspondences>cor(new pcl::Correspondences);
		core.determineReciprocalCorrespondences(*cor, 0.4);

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

		//pcl::io::savePCDFileASCII("overlapA.pcd", cloud_overlap1);
		//pcl::io::savePCDFileASCII("overlapB.pcd", cloud_overlap2);

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

		char normal_name1[40];
		sprintf_s(normal_name1, "%s%d%s", "normal_datas\\normal_", p, ".pcd");
		pcl::io::savePCDFileASCII(normal_name1, *nor_a);
		
		char normal_name2[40];
		sprintf_s(normal_name2, "%s%d%s", "normal_datas\\normal_", q, ".pcd");
		pcl::io::savePCDFileASCII(normal_name2, *nor_b);

		//p摄站下的中间法向量
		double angle_p = M_PI * (15 + (p - 1) * 30) / 180;
		double nor1_x = sin(angle_p);
		double nor1_y = 0;
		double nor1_z = cos(angle_p);

		//q摄站下的中间法向量
		double angle_q = M_PI * (15 + (q - 1) * 30) / 180;
		double nor2_x = sin(angle_q); 
		double nor2_y = 0;
		double nor2_z = cos(angle_q);

		pcl::PointCloud<pcl::PointXYZ>cloud_overlap;
		cloud_overlap.width = cor->size();
		cloud_overlap.height = 1;
		cloud_overlap.is_dense = false;
		cloud_overlap.resize(cloud_overlap.width*cloud_overlap.height);


		for (int j = 0; j < cor->size(); j++) {

			double nor_a_x = nor_a->points[j].normal_x;
			double nor_a_y = nor_a->points[j].normal_y;
			double nor_a_z = nor_a->points[j].normal_z;

			double nor_b_x = nor_b->points[j].normal_x;
			double nor_b_y = nor_b->points[j].normal_y;
			double nor_b_z = nor_b->points[j].normal_z;

			//点云法向量与摄站中线法向量的cosθ大小
			double res1 = (nor1_x * nor_a_x + nor1_z * nor_a_z) / (sqrt(nor1_x * nor1_x + nor1_z * nor1_z) * sqrt(nor_a_x * nor_a_x + nor_a_y * nor_a_y + nor_a_z * nor_a_z));
			double res2 = (nor2_x * nor_b_x + nor2_z * nor_b_z) / (sqrt(nor2_x * nor2_x + nor2_z * nor2_z) * sqrt(nor_b_x * nor_b_x + nor_b_y * nor_b_y + nor_b_z * nor_b_z));
			//debug << "res1=" << res1 << " ; " << "res2=" << res2 << endl;

			//cosθ值越大，夹角越小，保留夹角较小的点
			if (res1 > res2) {
				cloud_overlap.points[j].x = cloud_a->points[cor->at(j).index_query].x;
				cloud_overlap.points[j].y = cloud_a->points[cor->at(j).index_query].y;
				cloud_overlap.points[j].z = cloud_a->points[cor->at(j).index_query].z;

				cloud_b->points[cor->at(j).index_match].x = NULL;
				cloud_b->points[cor->at(j).index_match].y = NULL;
				cloud_b->points[cor->at(j).index_match].z = NULL;

			}
			else {
				cloud_overlap.points[j].x = cloud_b->points[cor->at(j).index_match].x;
				cloud_overlap.points[j].y = cloud_b->points[cor->at(j).index_match].y;
				cloud_overlap.points[j].z = cloud_b->points[cor->at(j).index_match].z;

				cloud_a->points[cor->at(j).index_query].x = NULL;
				cloud_a->points[cor->at(j).index_query].y = NULL;
				cloud_a->points[cor->at(j).index_query].z = NULL;

			}

			////原始点云去除重叠点，（这样会导致保留下来的重叠点无法继续与后面的点云做配准）
			//cloud_a->points[cor->at(j).index_query].x = NULL;
			//cloud_a->points[cor->at(j).index_query].y = NULL;
			//cloud_a->points[cor->at(j).index_query].z = NULL;

			//cloud_b->points[cor->at(j).index_match].x = NULL;
			//cloud_b->points[cor->at(j).index_match].y = NULL;
			//cloud_b->points[cor->at(j).index_match].z = NULL;
		}
		
		char overlap_name[40];
		sprintf_s(overlap_name, "%s%d%s", "result_datas\\overlap", p, ".pcd");
		pcl::io::savePCDFileASCII(overlap_name, cloud_overlap);

		char file_name3[30];
		sprintf_s(file_name3, "%s%d%s", "result_datas\\cloud", p, ".pcd");
		pcl::io::savePCDFileASCII(file_name3, *cloud_a);

		char file_name4[30];
		sprintf_s(file_name4, "%s%d%s", "result_datas\\cloud", q, ".pcd");
		pcl::io::savePCDFileASCII(file_name4, *cloud_b);
	}
	debug.close();
    return 0;
}
	
