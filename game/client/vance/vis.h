#pragma once

class Vector;

int GetClusterForOrigin( const Vector& org );
int GetPVSForCluster( int clusterIndex, int outputpvslength, unsigned char *outputpvs );
bool CheckOriginInPVS( const Vector& org, const unsigned char *checkpvs, int checkpvssize );

