//
//  ofxSvgLoader.h
//
//  Created by Nick Hardeman on 7/31/15.
//

#pragma once
#include "ofMain.h"
#include "ofxSvgGroup.h"

class ofxSvgLoader : public ofxSvgGroup {
public:
    ofxSvgLoader();
    
    bool load( string aPathToSvg );
    bool reload();
    
    void setFontsDirectory( string aDir );
    
    virtual string toString(int nlevel = 0);
    
    bool getTransformFromSvgMatrix( string matrix, ofVec2f& apos, float & scaleX, float & scaleY, float & arotation );
	
	const ofRectangle getBounds();
	const ofRectangle getViewbox();
	
protected:
    string fontsDirectory;
    string folderPath, svgPath;
    ofRectangle viewbox;
    ofRectangle bounds;
    void validateXmlSvgRoot( ofXml& aRootSvgNode );
    string cleanString( string aStr, string aReplace );
    void parseXmlNode( ofXml& aRootNode, ofXml& aParentNode, vector< shared_ptr<ofxSvgBase> >& aElements );
    bool addElementFromXmlNode( ofXml& aRootNode, ofXml& tnode, vector< shared_ptr<ofxSvgBase> >& aElements );
    void parseWithSvgTiny( ofXml& aRootNode, ofXml& tnode, shared_ptr<ofxSvgElement> aElement );
    void setupShape(struct svgtiny_shape * shape, ofPath & path);
    ofColor getColorFromXmlAttr( string aAtt );
    ofxSvgText::TextSpan getTextSpanFromXmlNode( ofXml& anode );
};









