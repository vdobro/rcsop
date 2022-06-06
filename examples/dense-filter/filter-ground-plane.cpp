#include "filter-ground-plane.h"

#include <pcl/ModelCoefficients.h>
#include <pcl/io/ply_io.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>

#include "points.h"

void separate_plane(const path& input_file, const path& output_file) {
    PointTCloudRef cloud(new PointTCloud);
    if (pcl::io::loadPLYFile(input_file.string(), *cloud) == -1) {
        const auto error_message = "Couldn't read file " + input_file.string() + "\n";
        PCL_ERROR (error_message.c_str());
        exit(-1);
    }
    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
    pcl::SACSegmentation<PointT> seg;
    // Optional
    seg.setOptimizeCoefficients(true);
    // Mandatory
    seg.setModelType(pcl::SACMODEL_PLANE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setDistanceThreshold(0.05);
    seg.setInputCloud(cloud);
    seg.segment(*inliers, *coefficients);

    if (inliers->indices.empty()) {
        PCL_ERROR ("Could not estimate a planar model for the given dataset.\n");
        exit(-1);
    }

    std::cerr << "Model coefficients: " << coefficients->values[0] << " "
              << coefficients->values[1] << " "
              << coefficients->values[2] << " "
              << coefficients->values[3] << std::endl;

    std::cerr << "Model inliers: " << inliers->indices.size() << std::endl;

    pcl::ExtractIndices<PointT> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(inliers);
    extract.setNegative(true);
    extract.filter(*cloud);

    pcl::io::savePLYFile(output_file.string(), *cloud, true);
}
