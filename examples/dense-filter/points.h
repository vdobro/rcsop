#ifndef SFM_COLORS_POINTS_H
#define SFM_COLORS_POINTS_H

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

typedef pcl::PointXYZRGBNormal PointT;
typedef pcl::PointCloud<PointT> PointTCloud;
typedef pcl::PointCloud<PointT>::Ptr PointTCloudRef;

#endif //SFM_COLORS_POINTS_H
