
#include "spatRaster.h"

#include "gdal_alg.h"
#include "ogrsf_frmts.h"


/*
std::vector<OGRGeometry *> geoms_from_ds(GDALDataset* src, int field, int value) {
	std::vector<OGRGeometry *> g;
	OGRLayer *poLayer = src->GetLayer(0);
	poLayer->ResetReading();
	OGRFeature *poFeature;
	
	while( (poFeature = poLayer->GetNextFeature()) != NULL ) {
		OGRGeometry *poGeometry = poFeature->GetGeometryRef();
		g.push_back(poGeometry);
	}
	return g;
}

union_cascated
// create output dataset 
	GDALDataset* dst;
// get unique values in field
// loop over unique values
	// for value in uvalues
	std::vector<OGRGeometry *> gvec = geoms_from_ds(src, field, value);
	OGRGeometry *geom;
	geom = (OGRGeometry *) gvec.data();
	OGRGeometry *gout;
	gout = geom->UnionCascaded();
// set geometry to output	
   return dst;
*/


SpatVector SpatRaster::polygonize(bool queen) {

	SpatVector out;
	SpatOptions opt;

	SpatRaster tmp = subset({0}, opt);

	GDALDatasetH rstDS;
	if (! tmp.sources_from_file() ) {
		if (!tmp.open_gdal(rstDS, 0)) {
			out.setError("cannot open dataset");
			return out;
		}
	} else {
		std::string filename = tmp.source[0].filename;
		rstDS = GDALOpen( filename.c_str(), GA_ReadOnly);
		if (rstDS == NULL) {
			out.setError("cannot open dataset from file");
			return out;			
		}
	}
    GDALDataset *srcDS;
	srcDS = srcDS->FromHandle(rstDS);

	std::vector<std::string> options;

    GDALDataset *poDS = NULL;
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName( "Memory" );
    if( poDriver == NULL )  {
        out.setError( "cannot create output dataset");
        return out;
    }
    poDS = poDriver->Create("", 0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL ) {
        out.setError("Creation of dataset failed" );
        return out;
    }
	std::vector<std::string> nms = getNames();
	std::string name = nms[0];

	OGRSpatialReference *SRS = NULL;
	std::string s = srs.wkt;
	if (s != "") {
		SRS = new OGRSpatialReference;
		OGRErr err = SRS->SetFromUserInput(s.c_str()); 
		if (err != OGRERR_NONE) {
			out.setError("crs error");
			delete SRS;
			return out;
		}
	}

    OGRLayer *poLayer;	
    poLayer = poDS->CreateLayer(name.c_str(), SRS, wkbPolygon, NULL );
    if( poLayer == NULL ) {
        out.setError( "Layer creation failed" );
        return out;
    }
	if (SRS != NULL) SRS->Release();

	OGRFieldDefn oField(name.c_str(), OFTInteger);
	if( poLayer->CreateField( &oField ) != OGRERR_NONE ) {
		out.setError( "Creating Name field failed");
		return out;
	}

	GDALRasterBand  *poBand;
	poBand = srcDS->GetRasterBand(1);

	//std::vector <char *> options_char = string_to_charpnt(options);

	CPLErr err = GDALPolygonize(poBand, NULL, poLayer, 0, NULL, NULL, NULL);
	if (err == 4) {
		out.setError("polygonize error");
		return out;
	}
	GDALClose(srcDS);

	out.read_ogr(poDS);
	GDALClose(poDS);

	out = out.aggregate(name, false);
	
	return out;
}
