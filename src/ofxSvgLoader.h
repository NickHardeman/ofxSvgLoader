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
    
    bool load( std::string aPathToSvg );
    bool reload();
    
    void setFontsDirectory( std::string aDir );
    
    virtual std::string toString(int nlevel = 0);
    
    bool getTransformFromSvgMatrix( std::string matrix, ofVec2f& apos, float & scaleX, float & scaleY, float & arotation );
	
	const ofRectangle getBounds();
	const ofRectangle getViewbox();
	
protected:
	std::string fontsDirectory;
	std::string folderPath, svgPath;
    ofRectangle viewbox;
    ofRectangle bounds;
    void validateXmlSvgRoot( ofXml& aRootSvgNode );
	std::string cleanString( std::string aStr, std::string aReplace );
    void parseXmlNode( ofXml& aRootNode, ofXml& aParentNode, std::vector< std::shared_ptr<ofxSvgBase> >& aElements );
    bool addElementFromXmlNode( ofXml& aRootNode, ofXml& tnode, std::vector< std::shared_ptr<ofxSvgBase> >& aElements );
    void parseWithSvgTiny( ofXml& aRootNode, ofXml& tnode, std::shared_ptr<ofxSvgElement> aElement );
    void setupShape(struct svgtiny_shape * shape, ofPath & path);
    ofColor getColorFromXmlAttr( std::string aAtt );
    ofxSvgText::TextSpan getTextSpanFromXmlNode( ofXml& anode );
};









