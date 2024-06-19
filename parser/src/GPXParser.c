// #include <stddef.h>
#include "GPXParser.h"

// javascript interaction
char* createGPXtoJSON(char* filename) {
  return GPXtoJSON(createGPXdoc(filename));
}
char* createGPXtoRoutesJSON(char* filename) {
  GPXdoc* doc = createGPXdoc(filename);
  if (doc == NULL) {
    printf("not a valid gpx");
    return "";
  }
  return routeListToJSON(doc->routes);
}
char* createGPXtoTracksJSON(char* filename) {
  GPXdoc* doc = createGPXdoc(filename);
  if (doc == NULL) {
    printf("not a valid gpx");
    return "";
  }
  return trackListToJSON(doc->tracks);
}



void traverseXml(xmlNode* aNode, void * object) {
      /* object is the parent node type struct*/
      if (object == NULL) {
      //  printf("null object\n" );
            return;
          }


      xmlNode *cur_node = NULL;
      xmlAttr *attr;

      for (cur_node = aNode; cur_node != NULL; cur_node = cur_node->next) {
      //  printf("name : %s, type : %d\n",cur_node->name, cur_node->type );
        if (cur_node->type == XML_ELEMENT_NODE) {
            //printf("node type: Element, name: %s\n", (char*) cur_node->name);
        //    printf("xml is node \n" );
            if (strcmp((char*) cur_node->name, "gpx") == 0) {
            // Iterate through every attribute of the current node
            GPXdoc* gpxdoc = (GPXdoc*) object; //check for error
            gpxdoc->creator = malloc(sizeof(char)*256);
            strcpy(gpxdoc->creator, "");
            xmlNs * ns = cur_node -> ns;
          //  char * namespace = (char*) ns->href;
            strcpy(gpxdoc->namespace,(char*) ns->href);
            for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
                  xmlNode *value = attr->children;
                  char *attrName = (char *)attr->name;
                  char *cont = (char *)(value->content);

                  if (strcmp(attrName, "version") == 0) {
                        gpxdoc->version = atof (cont);
                  }
                  else if (strcmp(attrName, "creator") == 0) {
                      //  gpxdoc->creator = (char*) realloc(gpxdoc->creator, sizeof(strlen(cont))+1);
                        strcpy(gpxdoc->creator, cont);
                  }
            }
            traverseXml(cur_node->children, (void*) gpxdoc);
       }
      else if (strcmp((char*) cur_node->name, "wpt") == 0 || strcmp((char*) cur_node->name, "rtept") == 0 || strcmp((char*) cur_node->name, "trkpt") == 0) {

            Waypoint* newPoint = malloc(sizeof(Waypoint));
            newPoint->name = malloc(sizeof(char)*500);
            strcpy(newPoint->name,"");
            newPoint->otherData = initializeList(&gpxDataToString,&deleteGpxData, &compareGpxData);

            for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
                  xmlNode *value = attr->children;
                  char *attrName = (char *)attr->name;
                  char *cont = (char *)(value->content);

                  if (strcmp(attrName, "lat") == 0) {
                        newPoint->latitude =  atof(cont);
                  }
                  if (strcmp(attrName, "lon") == 0) {
                        newPoint->longitude =  atof(cont);
                  }
            }


            xmlNode* parent = cur_node->parent;
            if (strcmp((char*) parent->name, "gpx") == 0) {
                  GPXdoc* gpxdoc = (GPXdoc*) object;
                  insertBack(gpxdoc->waypoints, (void*) newPoint);
            }
            if (strcmp((char*) parent->name, "trkseg") == 0) {
                  TrackSegment* segment = (TrackSegment*) object;
                  insertBack(segment->waypoints, (void*) newPoint);

            }
            if (strcmp((char*) parent->name, "rte") == 0) {
                  Route* route = (Route*) object;
                  insertBack(route->waypoints, (void*) newPoint);

            }

            traverseXml(cur_node->children, (void*) newPoint);
      }
      else if (strcmp((char*) cur_node->name, "trk") == 0) {

            Track* newTrack = (Track*) malloc(sizeof(Track));
            newTrack->name = (char*) malloc(sizeof(char)*500);
            strcpy(newTrack->name, "");
            newTrack->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
            newTrack->otherData = initializeList(&gpxDataToString,&deleteGpxData, &compareGpxData);
            GPXdoc* gpxdoc = (GPXdoc*) object;
            insertBack(gpxdoc->tracks, (void*) newTrack);

            traverseXml(cur_node->children, (void*) newTrack);
      }
      else if (strcmp((char*) cur_node->name, "trkseg") == 0) {

            TrackSegment * newSegment = malloc(sizeof(TrackSegment));
            newSegment->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
            Track* track = (Track*) object;
            insertBack(track->segments, (void*) newSegment);

            traverseXml(cur_node->children, (void*) newSegment);
      }

      else if (strcmp((char*) cur_node->name, "rte") == 0) {

            Route* newRoute = malloc(sizeof(Route));
            newRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
            newRoute->name = malloc(sizeof(char)*500);
            strcpy(newRoute->name, "");
            newRoute->otherData = initializeList(&gpxDataToString,&deleteGpxData, &compareGpxData);
            GPXdoc* gpxdoc = (GPXdoc*) object;
            insertBack(gpxdoc->routes, (void*) newRoute);

            traverseXml(cur_node->children, (void*) newRoute);

      }


      else if (strcmp((char*) cur_node->name, "name") == 0) {
            //printf("name node, cont: %s\n" , (cur_node->children)->content);

            if ((cur_node->children)->content != NULL) {

            xmlNode* parent = cur_node->parent;
        //    char* name = malloc(sizeof(char)* 500);
          //  strcpy(name,(char*)(cur_node->children)->content);
          char * name = (char*) (cur_node->children)->content;
          //  printf("%s\n", parent->name);
             if (strcmp((char*) parent->name, "trk") == 0) {
                  Track* track = (Track*) object;
                //  track->name = (char*) realloc(track->name, sizeof(strlen(name))+2);
                  strcpy(track->name, name);

            }
             if (strcmp((char*) parent->name, "trkpt") == 0 || strcmp((char*) parent->name, "rtept") == 0 || strcmp((char*) parent->name, "wpt") == 0) {


                  Waypoint* point = (Waypoint*) object;
                //  point->name = (char*) realloc(point->name, sizeof(strlen(name))+2);
                  strcpy(point->name, name);


              }

                if (strcmp((char*) parent->name, "rte") == 0) {
                  Route* route = (Route*) object;
                //  route->name = (char*) realloc(route->name, sizeof(strlen(name))+2);
                  strcpy(route->name, name);
                }

                //free(name);
         }

      }
      /* other data , desc , ele , etc.*/
      else {
            GPXData* newData = (GPXData*) malloc(sizeof(GPXData) + 500*sizeof(char));
            strcpy(newData->name, (char*) cur_node->name);
            if ( (cur_node->children)->content != NULL) {
              strcpy(newData->value, (char*) (cur_node->children)->content);
           }

            /* find parent struct*/

            xmlNode* parent = cur_node->parent;
            if (strcmp((char*) parent->name, "trk") == 0) {
                  Track* track = (Track*) object;
                  insertBack(track->otherData, (void*) newData);


            }
            else if (strcmp((char*) parent->name, "trkpt") == 0 || strcmp((char*) parent->name, "rtept") == 0 || strcmp((char*) parent->name, "wpt") == 0) {
                  Waypoint* point = (Waypoint*) object;
                  insertBack(point->otherData, (void*) newData);

            }
            else if (strcmp((char*) parent->name, "rte") == 0) {
                  Route* route = (Route*) object;
                  insertBack(route->otherData, (void*) newData);

            } else {


                  free(newData);

            }

      }


      }
    }
 return;
}



/** Function to create an GPX object based on the contents of an GPX file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createGPXdoc(char* fileName){

      xmlDoc *doc = NULL;
      xmlNode *root_element = NULL;
      doc = xmlReadFile(fileName, NULL, 0);

      if (doc == NULL) {
          //printf("could not open the file\n" );
            return NULL;
      }

      root_element = xmlDocGetRootElement(doc);
      GPXdoc * NewGPXdoc = malloc(sizeof(GPXdoc));
      NewGPXdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
      NewGPXdoc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
      NewGPXdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

      traverseXml(root_element, (void*) NewGPXdoc);

      xmlFreeDoc(doc);
      xmlCleanupParser();
      return NewGPXdoc;
}

/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc){

  if (doc == NULL) {
    return NULL;
  }
  GPXdoc* gpxdoc = (GPXdoc*) doc;

  char* pointsStr = toString(gpxdoc->waypoints);
  char* routesStr = toString(gpxdoc->routes);
  char* tracksStr = toString(gpxdoc->tracks);

  int len = strlen(gpxdoc->creator) + strlen(gpxdoc->namespace) + 355 + strlen(pointsStr)+ strlen(routesStr) + strlen(tracksStr) + 150;
  char* str = malloc(sizeof(char)*len);
  sprintf(str,"GPXdoc: creator : %s, version: %f, \n namespace: %s \n -GPXdoc Routes: ", gpxdoc->creator, gpxdoc->version, gpxdoc->namespace);
  strcat(str, routesStr);
  strcat(str,"\n -GPXdoc Tracks:");
  strcat(str,tracksStr);
  strcat(str,"\n -GPXdoc Waypoints:");
  strcat(str, pointsStr);

  free(routesStr);
  free(tracksStr);
  free(pointsStr);

  return str;

}

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/
void deleteGPXdoc(GPXdoc* doc){

  if (doc == NULL) {
    return;
  }
  GPXdoc* gpxdoc = (GPXdoc*) doc;
  freeList(gpxdoc->routes);
  freeList(gpxdoc->tracks);
  freeList(gpxdoc->waypoints);
  free(gpxdoc->creator);

  free(gpxdoc);

}

/* For the five "get..." functions below, return the count of specified entities from the file.
They all share the same format and only differ in what they have to count.

 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
 */


//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){

  if (doc == NULL) {
    return 0;
  }
  return getLength(doc->waypoints);
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){

  if (doc == NULL) {
    return 0;
  }
  return getLength(doc->routes);
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){

  if (doc == NULL) {
    return 0;
  }
  return getLength(doc->tracks);
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){

  if (doc == NULL) {
    return 0;
  }
  int numSegs = 0;
  ListIterator iter = createIterator(doc->tracks);
  void * elem;

  while ((elem = nextElement(&iter)) != NULL) {

    Track * trk = (Track*) elem;
    numSegs = numSegs + getLength(trk->segments);
  }
  return numSegs;
}

int getTrackSegmentNumData(TrackSegment* trkseg) {

  int numData = 0;
  ListIterator iter = createIterator(trkseg->waypoints);
  void* elem;
  while ((elem = nextElement(&iter)) != NULL) {

    Waypoint* pt = (Waypoint*) elem;
    if (strcmp(pt->name, "") != 0) {
      numData ++;
    }
    numData = numData + getLength(pt->otherData);
  }
  return numData;
}
//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc){

  if (doc == NULL) {
    return 0;
  }
  int numData = 0;
  ListIterator iter1 = createIterator(doc->waypoints);
  ListIterator iter2 = createIterator(doc->tracks);
  ListIterator iter3 = createIterator(doc->routes);
  void * elem;

  while ((elem = nextElement(&iter1)) != NULL) {

    Waypoint* pt = (Waypoint*) elem;
    if (strcmp(pt->name, "") != 0) {
      numData ++;
    }
    numData = numData + getLength(pt->otherData);
  }

    while ((elem = nextElement(&iter2)) != NULL) {

      Track* trk = (Track*) elem;
      if (strcmp(trk->name, "") != 0) {
        numData ++;
      }
      numData = numData + getLength(trk->otherData);
      ListIterator trkIter = createIterator(trk->segments);
      void * trkElem;

      while (( trkElem = nextElement(&trkIter)) != NULL) {

        TrackSegment* trkseg = (TrackSegment*) trkElem;
        numData = numData + getTrackSegmentNumData(trkseg);
      }

    }

    while ((elem = nextElement(&iter3)) != NULL) {

      Route* rte = (Route*) elem;
      if (strcmp(rte->name, "") != 0) {
        numData ++;
      }
      numData = numData + getLength(rte->otherData);
      ListIterator rteIter = createIterator(rte->waypoints);
      void * rteElem;
      while ((rteElem = nextElement(&rteIter)) != NULL) {

        Waypoint* rtept = (Waypoint*) rteElem;
        if (strcmp(rtept->name, "") != 0) {
          numData ++;
        }
        numData = numData + getLength(rtept->otherData);
      }

    }

    return numData;

}

bool compPointwithName(const void *first, const void *second){

  bool ret = false;
  if (first == NULL || second == NULL) {
    return ret;
  }
  Waypoint* point = (Waypoint*) first;
  char * str = (char*) second;

  if (strcmp((point->name), str) == 0) {
    ret = true;
    return ret;
  }
  else {
    return ret;
  }
}

bool compTrackwithName(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return false;
  }
  Track* trk = (Track*) first;
  char* str = (char*) second;

  if (strcmp((trk->name), str) == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool compRoutewithName(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return false;
  }
  Route* rte = (Route*) first;
  char* str = (char*) second;

  if (strcmp((rte->name), str) == 0) {
    return true;
  }
  else {
    return false;
  }
}
// Function that returns a waypoint with the given name.  If more than one exists, return the first one.
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

  if (doc == NULL || name == NULL) {
    return NULL;
  }
  Waypoint* ret = (Waypoint*) findElement(doc->waypoints, &compPointwithName, (void*) name);
  return ret;

}
// Function that returns a track with the given name.  If more than one exists, return the first one.
// Return NULL if the track does not exist
Track* getTrack(const GPXdoc* doc, char* name){

  if (doc == NULL || name == NULL) {
    return NULL;
  }

  Track* ret = (Track*) findElement(doc->tracks, &compTrackwithName, (void*) name);
  return ret;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name){

  if (doc == NULL || name == NULL) {
    return NULL;
  }

  Route* ret = (Route*) findElement(doc->routes, &compRoutewithName, (void*) name);
  return ret;

}




void deleteGpxData( void* data){

   if ( data == NULL) {
     return;
   }
  // printf("deleting data\n");
  GPXData* tmpData = (GPXData*) data;
  free(tmpData);
//  printf("data freed\n");
}

char* gpxDataToString( void* data){

  if( data == NULL) {
    return NULL;
  }

  GPXData* tmpData = (GPXData*) data;
  int len = strlen(tmpData->value) + strlen(tmpData->name) + 40;
  char* str = (char*) malloc(sizeof(char)*len);
  sprintf(str,"GPXData: name: %s, value:%s\n",tmpData->name, tmpData->value);

  return str;

}
int compareGpxData(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return 0;
  }
  GPXData* tmp1 = (GPXData*) first;
  GPXData* tmp2 = (GPXData*) second;

  return strcmp((char*) tmp1->value, (char*) tmp2->value);
}

void deleteWaypoint(void* data){

  if( data == NULL) {
    return ;
  }
  // printf("deleting waypoint\n");
  Waypoint* point = (Waypoint*) data;
  free(point->name);
  freeList(point->otherData);
  free(point);

}
char* waypointToString( void* data){

  if (data == NULL) {
    return NULL;
  }
  Waypoint* point = (Waypoint*) data;
  char* dataStr = toString(point->otherData);
  int len = 2*325 + strlen(point->name) + strlen(dataStr) + 60 ;                   //325 = max chars for a double
  char* str = malloc(sizeof(char)*len);
  sprintf(str,"Waypoint: name = %s, lat = %f, lon = %f \n other data:", point->name, point->latitude, point->longitude);
  strcat(str, dataStr);
  free(dataStr);

  return str;
}
//RETURN ONE IF ITS A MATCH
int compareWaypoints(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return 0;
  }
  Waypoint* point1 = (Waypoint*) first;
  Waypoint* point2 = (Waypoint*) second;

  if (strcmp((point1->name), (point2->name)) == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

void deleteRoute(void* data){

  if (data == NULL) {
    return;
  }
  // printf("deleting route\n");
  Route* rte = (Route*) data;

  freeList(rte->waypoints);
  freeList(rte->otherData);
  free(rte->name);
  free(rte);

}

char* routeToString(void* data){

  if (data == NULL) {
    return NULL;
  }

  Route* rte = (Route*) data;
  char* pointsStr = toString(rte->waypoints);
  char* dataStr = toString(rte->otherData);
  int len = strlen(rte->name) + strlen(pointsStr)+ strlen(dataStr) + 90 ;
  char* str = malloc(sizeof(char)*len);
  sprintf(str,"Route: name = %s \n -waypoints:", rte->name);
  strcat(str, pointsStr);
  strcat(str,"\n -Route other data:");
  strcat(str,dataStr);
  free(dataStr);
  free(pointsStr);

  return str;

}
int compareRoutes(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return 0;
  }
  Route* rte1 = (Route*) first;
  Route* rte2 = (Route*) second;

  if (strcmp((rte1->name), (rte2->name)) == 0) {
    return 1;
  }
  else {
    return 0;
  }

}

void deleteTrackSegment(void* data){

  if (data == NULL) {
    return;
  }
   //printf("deleting segment\n");
  TrackSegment* trkseg = (TrackSegment*) data;

  freeList(trkseg->waypoints);
  free(trkseg);

}
char* trackSegmentToString(void* data){

  if (data == NULL) {
    return NULL;
  }

  TrackSegment* trkseg = (TrackSegment*) data;
  char* pointsStr = toString(trkseg->waypoints);

  int len =  strlen(pointsStr)+  50 ;
  char* str = malloc(sizeof(char)*len);
  sprintf(str,"TrackSegment: \n -waypoints: ");
  strcat(str, pointsStr);
  free(pointsStr);

  return str;

}
int compareTrackSegments(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return 0;
  }
  TrackSegment* trkseg1 = (TrackSegment*) first;
  TrackSegment* trkseg2 = (TrackSegment*) second;

  if (trkseg1 == trkseg2) {
    return 0;
  } else {
    return 1;
  }

}

void deleteTrack(void* data){

  if (data == NULL) {
    return ;
  }

  Track* trk = (Track*) data;
  freeList(trk->segments);
  freeList(trk->otherData);
  free(trk->name);
  free(trk);

}
char* trackToString(void* data){

  if (data == NULL) {
    return NULL;
  }

  Track* trk = (Track*) data;
  char* segmentStr = toString(trk->segments);
  char* dataStr = toString(trk->otherData);
  int len = strlen(trk->name) + strlen(segmentStr)+ strlen(dataStr) + 90 ;
  char* str = malloc(sizeof(char)*len);
  sprintf(str,"Track: name = %s, \n -segments: ", trk->name);
  strcat(str, segmentStr);
  strcat(str,"\n -Track other data:\n");
  strcat(str,dataStr);
  free(dataStr);
  free(segmentStr);

  return str;

}
int compareTracks(const void *first, const void *second){

  if (first == NULL || second == NULL) {
    return 0;
  }
  Track* trk1 = (Track*) first;
  Track* trk2 = (Track*) second;

  if (strcmp((trk1->name), (trk2->name)) == 0) {
    return 1;
  }
  else {
    return 0;
  }
}


// A2 Module 1 Functions
int validateXml(char* fileName, char* gpxSchemaFile) {

xmlDocPtr doc;
xmlSchemaPtr schema = NULL;
xmlSchemaParserCtxtPtr ctxt;
int ret = -1;
LIBXML_TEST_VERSION;
xmlLineNumbersDefault(1);

ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
schema = xmlSchemaParse(ctxt);
xmlSchemaFreeParserCtxt(ctxt);

doc = xmlReadFile(fileName, NULL, 0);

if (doc == NULL)
{
 return 1;
}
else
{
xmlSchemaValidCtxtPtr ctxt;


ctxt = xmlSchemaNewValidCtxt(schema);
xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
ret = xmlSchemaValidateDoc(ctxt, doc);
xmlSchemaFreeValidCtxt(ctxt);
xmlFreeDoc(doc);
}
if(schema != NULL){
  xmlSchemaFree(schema);
}

xmlSchemaCleanupTypes();
xmlCleanupParser();
xmlMemoryDump();
return ret;
}

GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){

  if (validateXml(fileName, gpxSchemaFile) == 0) {
  //  printf("valid\n");
    return createGPXdoc(fileName);
  }
  else {
    //printf("not valid\n" );
    return NULL;
  }
}
void extractWaypoints(xmlNodePtr root_node, List* waypoints, char* pt ){
  xmlNodePtr node;
  char buff[512];
  ListIterator iter;
  ListIterator iter2;
  void * elem = NULL;
  void * elem2 = NULL;
  iter = createIterator(waypoints);
  while ((elem = nextElement(&iter)) != NULL) {
    Waypoint* wpt = (Waypoint*) elem;
    node = xmlNewChild(root_node, NULL, BAD_CAST pt,NULL);
    sprintf(buff, "%f", wpt->latitude);
    xmlNewProp(node, BAD_CAST "lat", BAD_CAST buff);
    sprintf(buff, "%f", wpt->longitude);
    xmlNewProp(node, BAD_CAST "lon", BAD_CAST buff);
    if (strcmp(wpt->name, "")!= 0){
      xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST wpt->name);
    }
    iter2 = createIterator(wpt->otherData);
    while ((elem2 = nextElement(&iter2)) != NULL) {
      GPXData* data = (GPXData*) elem2;
      xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
    }
}
}
int GPXdocToXml(GPXdoc* gpxdoc, char* fileName) {

if (gpxdoc == NULL){
  return 0;
}
//validate constraints first
xmlDocPtr doc = NULL;       /* document pointer */
xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */

ListIterator iter;
ListIterator iter2;
void * elem = NULL;
void * elem2 = NULL;
char buff[512];



 LIBXML_TEST_VERSION;
 doc = xmlNewDoc(BAD_CAST "1.0");
 root_node = xmlNewNode(NULL, BAD_CAST "gpx");
  xmlDocSetRootElement(doc, root_node);
  xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST gpxdoc->namespace, NULL);
  xmlSetNs(root_node, ns);
  sprintf(buff, "%.1f", gpxdoc->version);
  xmlNewProp(root_node, BAD_CAST "version", BAD_CAST buff );
  xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST gpxdoc->creator);

  // wpt
  extractWaypoints(root_node, gpxdoc->waypoints, "wpt");

 // rte
  iter = createIterator(gpxdoc->routes);
  while ((elem = nextElement(&iter)) != NULL) {
     Route* rte = (Route*) elem;
     node = xmlNewChild(root_node, NULL, BAD_CAST "rte",NULL);
     if (strcmp(rte->name, "")!= 0){
       xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST rte->name);
     }
     iter2 = createIterator(rte->otherData);
     while ((elem2 = nextElement(&iter2)) != NULL) {
       GPXData* data = (GPXData*) elem2;
       xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
     }
     extractWaypoints(node, rte->waypoints, "rtept");
  }

  //trk
  iter = createIterator(gpxdoc->tracks);
  while ((elem = nextElement(&iter)) != NULL) {

   Track* trk = (Track*) elem;
   node = xmlNewChild(root_node, NULL, BAD_CAST "trk",NULL);
   if (strcmp(trk->name, "")!= 0){
     xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST trk->name);
   }
   iter2 = createIterator(trk->otherData);
   while ((elem2 = nextElement(&iter2)) != NULL) {
     GPXData* data = (GPXData*) elem2;
     xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
   }
   iter2 = createIterator(trk->segments);
   while ((elem2 = nextElement(&iter2)) != NULL) {
    TrackSegment* trkseg = (TrackSegment*) elem2;
    node1 = xmlNewChild(node, NULL, BAD_CAST "trkseg",NULL);
    extractWaypoints(node1, trkseg->waypoints, "trkpt");
  }

  }
  xmlSaveFormatFileEnc(fileName, doc, "UTF-8",1);
  xmlFreeDoc(doc);
  xmlCleanupParser();

 return 1;
}
int validateWaypoints(List* waypoints){
  ListIterator iter;
  ListIterator iter2;
  void * elem = NULL;
  void * elem2 = NULL;
  iter = createIterator(waypoints);
  while ((elem = nextElement(&iter)) != NULL) {
    Waypoint* wpt = (Waypoint*) elem;
    if (wpt->name == NULL) {
      return 0;
    }
    if (wpt->otherData == NULL) {
      return 0;
    }
    iter2 = createIterator(wpt->otherData);
    while ((elem2 = nextElement(&iter2)) != NULL) {
      GPXData* data = (GPXData*) elem2;
      if (strcmp(data->name,"") == 0 || strcmp(data->value,"") == 0) {
        return 0;
      }
    }
  }
  return 1;

}
int validateRoutes(List* routes){
  ListIterator iter;
  ListIterator iter2;
  void * elem = NULL;
  void * elem2 = NULL;
  iter = createIterator(routes);
  while ((elem = nextElement(&iter)) != NULL) {
    Route* rte = (Route*) elem;
    if (rte->name == NULL) {
      return 0;
    }
    if (rte->otherData == NULL) {
      return 0;
    }
    iter2 = createIterator(rte->otherData);
    while ((elem2 = nextElement(&iter2)) != NULL) {
      GPXData* data = (GPXData*) elem2;
      if (strcmp(data->name,"") == 0 || strcmp(data->value,"") == 0) {
        return 0;
      }
    }
    if (rte->waypoints == NULL) {
      return 0;
    }
    if (validateWaypoints(rte->waypoints) == 0) {
      return 0;
    }

  }
  return 1;
}
int validateTracks(List* tracks) {
  ListIterator iter;
  ListIterator iter2;
  void * elem = NULL;
  void * elem2 = NULL;
  iter = createIterator(tracks);
  while ((elem = nextElement(&iter)) != NULL) {

   Track* trk = (Track*) elem;
   if (trk->name == NULL){
     return 0;
   }
   if (trk->otherData == NULL){
     return 0;
   }
   iter2 = createIterator(trk->otherData);
   while ((elem2 = nextElement(&iter2)) != NULL) {
     GPXData* data = (GPXData*) elem2;
     if (strcmp(data->name,"") == 0 || strcmp(data->value,"") == 0) {
       return 0;
     }
   }

   if (trk->segments == NULL){
     return 0;
   }
   iter2 = createIterator(trk->segments);
   while ((elem2 = nextElement(&iter2)) != NULL) {
     TrackSegment* trkseg = (TrackSegment*) elem2;
     if (trkseg->waypoints == NULL) {
       return 0;
     }
     if (validateWaypoints(trkseg->waypoints) == 0) {
       return 0;
     }
   }
 }
 return 1;
}
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){
  bool ret = true;
  if (doc == NULL || gpxSchemaFile == NULL) {
    return false;
  }
  // check constraints
  if (doc->creator == NULL) {
    return false;
  }
 if (strcmp(doc->namespace,"") == 0) {
   return false;
 }
 if (doc->waypoints == NULL) {
   return false;
 }
 if (validateWaypoints(doc->waypoints) == 0){
   return false;
 }
 if (doc->routes == NULL) {
   return false;
 }
 if (validateRoutes(doc->routes) == 0){
   return false;
 }
 if (doc->tracks == NULL) {
   return false;
 }
 if (validateTracks(doc->tracks) == 0){
   return false;
 }


  // validate with schema file
  GPXdocToXml(doc, "test.gpx");
  if (validateXml("test.gpx", gpxSchemaFile) != 0){
    remove("test.gpx");
    ret = false;
  }
  return ret;
}

bool writeGPXdoc(GPXdoc* doc, char* fileName){
  bool ret = false;
  if (doc == NULL || fileName == NULL) {
    return ret;
  }

  if (GPXdocToXml(doc, fileName) == 1){
    ret = true;
  }
 return ret;
}

// A2 Module functions
float getDistance(double lat1, double lon1, double lat2, double lon2){
  float r = 6371000;
  double radLat1 = (double) lat1*(M_PI/180);
  double radLat2 = (double) lat2*(M_PI/180);
  double diffLat = (double) (lat2 - lat1)*(M_PI/180);
  double diffLon = (double) (lon2 - lon1)*(M_PI/180);
  double a = (double) sin(diffLat/2)*sin(diffLat/2) + cos(radLat1)*cos(radLat2)*sin(diffLon/2)*sin(diffLon/2);
  double c = (double) 2*atan2(sqrt(a), sqrt(1-a));
  float dist = (float) r*c;
  return dist;
}
float round10(float len){

  float ret = 0;
  int div = len/10;
  float mod = len - (div*10);
  if (mod >= 5) {
    ret = (div*10) + 10;
  }
  else {
    ret = (div*10);
  }
  return ret;
}
float getWaypointsLen(List* waypoints) {
  float len = 0;
  ListIterator iter;
  ListIterator iter2;
  void * elem = NULL;
  void * elem2 = NULL;
  iter = createIterator(waypoints);
  iter2 = createIterator(waypoints);
  nextElement(&iter2);
  while ((elem2 = nextElement(&iter2)) != NULL) {
      elem = nextElement(&iter);
      Waypoint* wpt1 = (Waypoint*) elem;
      Waypoint* wpt2 = (Waypoint*) elem2;
      len += (float) getDistance(wpt1->latitude, wpt1->longitude, wpt2->latitude, wpt2->longitude);
  }
  return len;
}

float getRouteLen(const Route *rt) {
  if (rt == NULL) {
    return 0;
  }
  Route* rte = (Route*) rt;
  float len = 0;
  len = (float) getWaypointsLen(rte->waypoints);
  return len;
}

float getTrackLen(const Track *tr){
  if (tr == NULL) {
    return 0;
  }
  Track* trk = (Track*) tr;

  float len = 0;
  ListIterator iter;
  ListIterator iter2;
  ListIterator iter3;
  void * elem = NULL;
  void * elem2 = NULL;
  void * elem3 = NULL;
  iter = createIterator(trk->segments);
  iter2 = createIterator(trk->segments);
  iter3 = createIterator(trk->segments);

  while ((elem = nextElement(&iter)) != NULL) {
    TrackSegment* seg1 = (TrackSegment*) elem;
    len += (float) getWaypointsLen(seg1->waypoints);
  }
  // more than one segment
   nextElement(&iter3);
   while((elem3 = nextElement(&iter3)) != NULL) {
     elem2 = nextElement(&iter2);
     TrackSegment* seg2 = (TrackSegment*) elem2;
     TrackSegment* seg3 = (TrackSegment*) elem3;
     Waypoint* wpt1 = (Waypoint*) getFromBack(seg2->waypoints);
     Waypoint* wpt2 = (Waypoint*) getFromFront(seg3->waypoints);
     len += (float) getDistance(wpt1->latitude, wpt1->longitude, wpt2->latitude, wpt2->longitude);

   }

  return len;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
  if ( doc == NULL || len < 0 || delta < 0) {
    return 0;
  }
  int num = 0;
  float diff = 0;
  GPXdoc* gpxdoc = (GPXdoc*) doc;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(gpxdoc->routes);

  while ((elem = nextElement(&iter)) != NULL) {
    Route* rte = (Route*) elem;
    diff = (float) fabs(getRouteLen(rte) - len);
    if (diff <= delta) {
      num ++;
    }
  }
  return num;

}

int numTracksWithLength(const GPXdoc* doc, float len, float delta){

  if ( doc == NULL || len < 0 || delta < 0) {
    return 0;
  }
  int num = 0;
  float diff = 0;
  GPXdoc* gpxdoc = (GPXdoc*) doc;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(gpxdoc->tracks);

  while ((elem = nextElement(&iter)) != NULL) {
    Track* trk = (Track*) elem;
    diff = (float) fabs(getTrackLen(trk) - len);
    if (diff <= delta) {
      num ++;
    }
  }
  return num;
}

bool isLoopRoute(const Route* route, float delta){
   bool ret = true;
  if (route == NULL || delta < 0) {
    return false;
  }
  Route* rte = (Route*) route;

  if (getLength(rte->waypoints) < 4) {
    return false;
  }

  Waypoint* wpt1 = (Waypoint*) getFromFront(rte->waypoints);
  Waypoint* wpt2 = (Waypoint*) getFromBack(rte->waypoints);
  float len = (float) getDistance(wpt1->latitude, wpt1->longitude, wpt2->latitude, wpt2->longitude);
  if ( len > delta) {
    return false;
  }
  return ret;
}

bool isLoopTrack(const Track *tr, float delta){

bool ret = true;
 if (tr == NULL || delta < 0) {
   return false;
 }
  int numPoints = 0;
  Track* trk = (Track*) tr;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(trk->segments);
  while ((elem = nextElement(&iter)) != NULL) {
    TrackSegment* seg = (TrackSegment*) elem;
    numPoints += getLength(seg->waypoints);
  }
  if (numPoints < 4) {
    return false;
  }
  TrackSegment* seg1 = (TrackSegment*) getFromFront(trk->segments);
  TrackSegment* seg2 = (TrackSegment*) getFromBack(trk->segments);
  Waypoint* wpt1 = (Waypoint*) getFromFront(seg1->waypoints);
  Waypoint* wpt2 = (Waypoint*) getFromBack(seg2->waypoints);
  float len = (float) getDistance(wpt1->latitude, wpt1->longitude, wpt2->latitude, wpt2->longitude);
  if ( len > delta) {
    return false;
  }
  return ret;


}

void dummyDelete(void* data) {
  return;
}
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
  if (doc == NULL){
    return NULL;
  }
  List* routes = NULL;
  routes = initializeList(&routeToString, &dummyDelete, &compareRoutes);
  float len1 = 0;
  float len2 = 0;
  GPXdoc* gpxdoc = (GPXdoc*) doc;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(gpxdoc->routes);
  while ((elem = nextElement(&iter)) != NULL) {
    Route* rte = (Route*) elem;
    Waypoint* wpt1 = (Waypoint*) getFromFront(rte->waypoints);
    Waypoint* wpt2 = (Waypoint*) getFromBack(rte->waypoints);
    len1 = (float) getDistance(wpt1->latitude, wpt1->longitude, sourceLat, sourceLong);
    len2 = (float) getDistance(wpt2->latitude, wpt2->longitude, destLat, destLong);
    if (len1 <= delta && len2 <= delta) {
      insertBack(routes, rte);
    }
  }

  if (getLength(routes) == 0){
    freeList(routes);
    return NULL;
  }
  else {
    return routes;
  }
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

  if (doc == NULL){
    return NULL;
  }
  List* tracks = NULL;
  tracks = initializeList(&trackToString, &dummyDelete, &compareTracks);
  float len1 = 0;
  float len2 = 0;
  GPXdoc* gpxdoc = (GPXdoc*) doc;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(gpxdoc->tracks);
  while ((elem = nextElement(&iter)) != NULL) {
    Track* trk = (Track*) elem;
    TrackSegment* firstSeg = (TrackSegment*) getFromFront(trk->segments);
    TrackSegment* lastSeg = (TrackSegment*) getFromBack(trk->segments);
    Waypoint* wpt1 = (Waypoint*) getFromFront(firstSeg->waypoints);
    Waypoint* wpt2 = (Waypoint*) getFromBack(lastSeg->waypoints);
    len1 = (float) getDistance(wpt1->latitude, wpt1->longitude, sourceLat, sourceLong);
    len2 = (float) getDistance(wpt2->latitude, wpt2->longitude, destLat, destLong);
    if (len1 <= delta && len2 <= delta) {
      insertBack(tracks, trk);
    }
  }

  if (getLength(tracks) == 0){
    freeList(tracks);
    return NULL;
  }
  else {
    return tracks;
  }
}


//Module 3

int getTrackPoints(Track* trk) {
  int numPoints = 0;
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(trk->segments);
  while ((elem = nextElement(&iter)) != NULL) {
    TrackSegment* seg = (TrackSegment*) elem;
    numPoints += getLength(seg->waypoints);
  }
  return numPoints;

}

char* trackToJSON(const Track *tr){

  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (tr == NULL) {
    strcpy(str, "{}");
    return str;
  }
  Track* trk = (Track*) tr;
  char isLoop[10];
  if (isLoopTrack(trk,10)) {
    strcpy(isLoop, "true");
  } else {
    strcpy(isLoop, "false");
  }

  len += strlen(trk->name) + 1;
  str = (char*) realloc(str, sizeof(char)*(len));
  float trkLen = (float) round10(getTrackLen(trk));
  sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", trk->name, getTrackPoints(trk), trkLen, isLoop);
  return str;

}
char* otherDataToJSON(GPXData* data) {
  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (data == NULL) {
    strcpy(str, "{}");
    return str;
  }
    sprintf(str,"{\"name\":\"%s\",\"value\":%s}", data->name, data->value);
    return str;

}
char* otherDataListToJSON(List* list) {
  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (list == NULL) {
    strcpy(str, "[]");
    return str;
  }
  
  sprintf(str, "[");
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(list);
  int i = 0;
  while ((elem = nextElement(&iter)) != NULL) {
    if ( i != 0) {
      strcat(str,",");
    }
    i++;
    GPXData* data = (GPXData*) elem;
    char* rteStr = otherDataToJSON(data);
    len += strlen(rteStr) + 1;
    str = (char*) realloc(str, sizeof(char)*(len));
    strcat(str, rteStr);
    free(rteStr);
  }
  strcat(str,"]");
  return str;

}
char* routeToJSON(const Route *rt){
  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (rt == NULL) {
    strcpy(str, "{}");
    return str;
  }
  Route* rte = (Route*) rt;
  char isLoop[10];
  if (isLoopRoute(rte,10)) {
    strcpy(isLoop, "true");
  } else {
    strcpy(isLoop, "false");
  }
  len += strlen(rte->name) + 1;
  str = (char*) realloc(str, sizeof(char)*(len));
  //gpxdoc->creator = (char*) realloc(gpxdoc->creator, sizeof(strlen(cont))+1);
//  int len = strlen(rte->name) + strlen(pointsStr)+ strlen(dataStr) + 90 ;
  float rteLen = (float) round10(getRouteLen(rte));
  sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", rte->name, getLength(rte->waypoints),rteLen, isLoop);
  return str;
}

char* routeListToJSON(const List *list){

  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (list == NULL) {
    strcpy(str, "[]");
    return str;
  }
  List* routes = (List*) list;
  sprintf(str, "[");
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(routes);
  int i = 0;
  while ((elem = nextElement(&iter)) != NULL) {
    if ( i != 0) {
      strcat(str,",");
    }
    i++;
    Route* rte = (Route*) elem;
    char* rteStr = routeToJSON(rte);
    len += strlen(rteStr) + 1;
    str = (char*) realloc(str, sizeof(char)*(len));
    strcat(str, rteStr);
    free(rteStr);
  }
  strcat(str,"]");
  return str;


}

char* trackListToJSON(const List *list){
  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (list == NULL) {
    strcpy(str, "[]");
    return str;
  }
  List* tracks = (List*) list;
  sprintf(str, "[");
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(tracks);
  int i = 0;
  while ((elem = nextElement(&iter)) != NULL) {
    if ( i != 0) {
      strcat(str,",");
    }
    i++;
    Track* trk = (Track*) elem;
    char* trkStr = trackToJSON(trk);
    len += strlen(trkStr) + 1;
    str = (char*) realloc(str, sizeof(char)*(len));
    strcat(str, trkStr);
    free(trkStr);
  }
  strcat(str,"]");
  return str;
}

char* GPXtoJSON(const GPXdoc* gpx){
  int len = 512;
  char* str  = malloc(sizeof(char)*len);
  if (gpx == NULL) {
    strcpy(str, "{}");
    return str;
  }
  GPXdoc* doc = (GPXdoc*) gpx;

  len += strlen(gpx->creator) + 1;
  str = (char*) realloc(str, sizeof(char)*(len));

  sprintf(str,"{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",
   doc->version, doc->creator, getNumWaypoints(doc), getNumRoutes(doc), getNumTracks(doc));
  return str;

}

char* rtePtToJSON(char* filename) {

  GPXdoc* doc = createGPXdoc(filename);
  if (doc == NULL) {
    printf("not a valid gpx");
    return "";
  }

      int len = 512;
    char* str  = malloc(sizeof(char)*len);
    if (doc->routes == NULL) {
      strcpy(str, "[]");
      return str;
    }
    List* routes = doc->routes;
    sprintf(str, "[");
    ListIterator iter;
    void * elem = NULL;
    iter = createIterator(routes);
    int i = 0;
    int routeIdx = 1;
    int pointIdx = 1;
    while ((elem = nextElement(&iter)) != NULL) {

      Route* rte = (Route*) elem;
      ListIterator iter2;
      void * elem2 = NULL;
      iter2 = createIterator(rte->waypoints);
      pointIdx = 1;
      while ((elem2 = nextElement(&iter2)) != NULL) {
          char ptStr[600];
          Waypoint* point = (Waypoint*) elem2;
          sprintf(ptStr,"{\"point_index\":%d,\"latitude\":%f,\"longitude\":%f,\"name\":\"%s\",\"route_id\":%d}",pointIdx, point->latitude, point->longitude, point->name, routeIdx);
          len += strlen(ptStr) + 1;
          pointIdx++;
          if ( i != 0) {
            strcat(str,",");
          }
          i++;
          str = (char*) realloc(str, sizeof(char)*(len));
          strcat(str, ptStr);
      }
   
      routeIdx ++;
    }
    strcat(str,"]");
    printf("%s", str);
    return str;

}

char* routesOtherDataToJSON(char* filename) {

   GPXdoc* doc = createGPXdoc(filename);
  if (doc == NULL) {
    printf("not a valid gpx");
    return "";
  }
    int len = 512;
  char* str  = malloc(sizeof(char)*len);
  List* routes = doc->routes;
  sprintf(str, "[");
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(routes);
  int i = 0;
  while ((elem = nextElement(&iter)) != NULL) {
    if ( i != 0) {
      strcat(str,",");
    }
    i++;
    Route* rte = (Route*) elem;
    char* rteStr = otherDataListToJSON(rte->otherData);
    len += strlen(rteStr) + 1;
    str = (char*) realloc(str, sizeof(char)*(len));
    strcat(str, rteStr);
    free(rteStr);
  }
  strcat(str,"]");

  return str;
  
}
char* tracksOtherDataToJSON(char* filename) {

   GPXdoc* doc = createGPXdoc(filename);
  if (doc == NULL) {
    printf("not a valid gpx");
    return "";
  }
    int len = 512;
  char* str  = malloc(sizeof(char)*len);

  List* tracks = doc->tracks;
  sprintf(str, "[");
  ListIterator iter;
  void * elem = NULL;
  iter = createIterator(tracks);
  int i = 0;
  while ((elem = nextElement(&iter)) != NULL) {
    if ( i != 0) {
      strcat(str,",");
    }
    i++;
    Track* trk = (Track*) elem;
    char* trkStr = otherDataListToJSON(trk->otherData);
    len += strlen(trkStr) + 1;
    str = (char*) realloc(str, sizeof(char)*(len));
    strcat(str, trkStr);
    free(trkStr);
  }
  strcat(str,"]");
  return str;

  
  
}


// ***************************** Bonus A2 functions ********************************


void addWaypoint(Route *rt, Waypoint *pt){
  if (rt == NULL || pt == NULL) {
      return ;
  }
  insertBack(rt->waypoints, pt);
}

void addRoute(GPXdoc* doc, Route* rt){
  if (doc == NULL || rt == NULL) {
    return;
  }
  insertBack(doc->routes, rt);
}

GPXdoc* JSONtoGPX(const char* gpxString){

  if (gpxString == NULL) {
    return NULL;
  }
  GPXdoc * NewGPXdoc = malloc(sizeof(GPXdoc));
  NewGPXdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  NewGPXdoc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
  NewGPXdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
  strcpy(NewGPXdoc->namespace,"http://www.topografix.com/GPX/1/1");

  char* str = (char*) gpxString;
  char* token1;
  char* token2;
  token1 = strtok(str, ",");
  char* ver = malloc(sizeof(char)*100);
  strcpy(ver, token1);
  printf("%s\n",ver );
  token1 = strtok(NULL, ",");
  char * creator = malloc(sizeof(char)*300);
  strcpy(creator, token1);
//  printf("%s\n",creator );
  token2 = strtok(ver,":");
  token2 = strtok(NULL, ":");
  strcpy(ver, token2);
  sscanf( ver,"%lf", &NewGPXdoc->version);
  free(ver);


  token2 = strtok(creator,":");
  token2 = strtok(NULL, ":");
  strcpy(creator, token2);
  token1 = strtok(creator, "\"}");
  strcpy(creator,token1);
  NewGPXdoc->creator = malloc(sizeof(char)*(strlen(creator)+1));
  strcpy(NewGPXdoc->creator, creator);
  free(creator);

  return NewGPXdoc;
  }






Waypoint* JSONtoWaypoint(const char* gpxString){
  if (gpxString == NULL) {
    return NULL;
  }
  Waypoint* newPoint = malloc(sizeof(Waypoint));
  newPoint->name = malloc(sizeof(char)*500);
  strcpy(newPoint->name,"");
  newPoint->otherData = initializeList(&gpxDataToString,&deleteGpxData, &compareGpxData);

  char* str = (char*) gpxString;
  char* token1;
  char* token2;
  token1 = strtok(str, ",");
  char* lat = malloc(sizeof(char)*100);
  strcpy(lat, token1);
  token1 = strtok(NULL, ",");
  char * lon = malloc(sizeof(char)*300);
  strcpy(lon, token1);
  token2 = strtok(lat,":");
  token2 = strtok(NULL, ":");
  strcpy(lat, token2);
  sscanf( lat,"%lf", &(newPoint->latitude));
  free(lat);

  token2 = strtok(lon,":");
  token2 = strtok(NULL, ":");
  strcpy(lon, token2);
  sscanf( lon,"%lf", &(newPoint->longitude));
  free(lon);

  return newPoint;

}

Route* JSONtoRoute(const char* gpxString){

  if (gpxString == NULL) {
    return NULL;
  }
  Route* newRoute = malloc(sizeof(Route));
  newRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  newRoute->name = malloc(sizeof(char)*500);
  newRoute->otherData = initializeList(&gpxDataToString,&deleteGpxData, &compareGpxData);

  char* str = (char*) gpxString;
  char* token1;
  char* token2;
  token1 = strtok(str, ":");
  token1 = strtok( NULL, ":");
  char* name = malloc(sizeof(char)*200);
  strcpy(name, token1);
  token2 = strtok(name, "\"");
  strcpy(name, token2);
  strcpy(newRoute->name, name);
  free(name);

  return newRoute;
}
