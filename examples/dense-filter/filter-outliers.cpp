#include "filter-outliers.h"

#include <pcl/io/ply_io.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/passthrough.h>

#include "points.h"

void filter_outliers(const path& input_file,
                     const path& output_file) {
    PointTCloudRef cloud(new PointTCloud);
    if (pcl::io::loadPLYFile(input_file.string(), *cloud) == -1) {
        const auto error_message = "Couldn't read file " + input_file.string() + "\n";
        PCL_ERROR (error_message.c_str());
        exit(-1);
    }

    pcl::IndicesPtr indices (new std::vector <int>);
    pcl::removeNaNFromPointCloud(*cloud, *indices);

    pcl::ExtractIndices<PointT> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(indices);
    extract.setNegative(false);
    extract.filter(*cloud);

    pcl::PassThrough<PointT> x_pass;
    x_pass.setInputCloud(cloud);
    x_pass.setFilterFieldName("x");
    x_pass.setFilterLimits(-1.0, 1.15); // Länge
    x_pass.filter(*cloud);

    pcl::PassThrough<PointT> y_pass;
    y_pass.setInputCloud(cloud);
    y_pass.setFilterFieldName("y");
    y_pass.setFilterLimits(-0.5, 0.25); // Höhe
    y_pass.filter(*cloud);

    pcl::PassThrough<PointT> z_pass;
    z_pass.setInputCloud(cloud);
    z_pass.setFilterFieldName("z");
    z_pass.setFilterLimits(-0.45, 0.58); // Breite
    z_pass.filter(*cloud);

    pcl::io::savePLYFile(output_file.string(), *cloud, true);
}
