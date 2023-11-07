//
//  ofxSvgBase.h
//
//  Created by Nick Hardeman on 7/31/15.
//

#pragma once
#include "ofMain.h"
#include <map>

class ofxSvgBase {
public:
    enum Types {
        OFX_SVG_TYPE_GROUP = 0,
        OFX_SVG_TYPE_RECTANGLE,
        OFX_SVG_TYPE_IMAGE,
        OFX_SVG_TYPE_ELLIPSE,
        OFX_SVG_TYPE_CIRCLE,
        OFX_SVG_TYPE_PATH,
        OFX_SVG_TYPE_TEXT,
        OFX_SVG_TYPE_ELEMENT,
        OFX_SVG_TYPE_TOTAL
    };
    
    ofxSvgBase() { name = "No Name"; type = OFX_SVG_TYPE_TOTAL; setVisible( true ); }
    
    int getType() {return type;}
    std::string getTypeAsString();
    
	std::string getName() { return name; }
    virtual int getNumChildren() { return 0; }
    bool isGroup() {
        return (getType() == OFX_SVG_TYPE_GROUP);
    }
    
    void setVisible( bool ab ) { bVisible = ab; }
    bool isVisible() { return bVisible; }
    
    virtual void setUseShapeColor( bool ab ) {
        bUseShapeColor = ab;
    }
    
    virtual void draw() {}
    
    virtual std::string toString(int nlevel = 0);
    
    virtual glm::mat4 getTransformMatrix();
    virtual ofNode getNodeTransform();
    
	std::string name;
    int type;
    bool bVisible;
    ofVec2f pos;
    bool bUseShapeColor = true;
};

class ofxSvgElement : public ofxSvgBase {
public:
    
    ofxSvgElement() {scale.set(1,1); rotation = 0.0; type=OFX_SVG_TYPE_ELEMENT;}
    
    ofVec2f scale = ofVec2f(1.0f, 1.0f);
    float rotation = 0.0f;
    
    virtual glm::mat4 getTransformMatrix() override;
    virtual ofNode getNodeTransform() override;
    
    ofPath path;
    
    virtual void draw() override {
        if(isVisible()) path.draw();
    }
    
    bool isFilled() { return path.isFilled(); }
    bool hasStroke() { return path.hasOutline(); }
    float getStrokeWidth() { return path.getStrokeWidth(); }
    ofColor getFillColor() { return path.getFillColor(); }
    ofColor getStrokeColor() { return path.getStrokeColor(); }
    
    void setUseShapeColor( bool ab ) override {
        ofxSvgBase::setUseShapeColor(ab);
        path.setUseShapeColor(ab);
    }
};

class ofxSvgRectangle : public ofxSvgElement {
public:
    ofxSvgRectangle() { type = OFX_SVG_TYPE_RECTANGLE; }
    ofRectangle rectangle;
    
    float getWidth() { return rectangle.getWidth();}
    float getHeight() { return rectangle.getHeight();}
    
    float getWidthScaled() { return rectangle.getWidth() * scale.x;}
    float getHeightScaled() { return rectangle.getHeight() * scale.y;}
};

class ofxSvgImage : public ofxSvgRectangle {
public:
    ofxSvgImage() { type = OFX_SVG_TYPE_IMAGE; bTryLoad = false; }
    
    virtual void draw() {
        if( !bTryLoad ) {
            img.load( getFilePath() );
            bTryLoad = true;
        }
        
        if( isVisible() ) {
            if( img.isAllocated() ) {
                ofPushMatrix(); {
                    ofTranslate( pos.x, pos.y );
                    if( rotation != 0.0 ) ofRotateZDeg( rotation );
                    ofScale( scale.x, scale.y );
                    if(bUseShapeColor) ofSetColor( getColor() );
                    img.draw( 0, 0 );
                } ofPopMatrix();
            }
        }
    }
    
	std::string getFilePath() { return filepath; }
    
    void setColor( ofColor aColor ) {
        color = aColor;
    }
    ofColor getColor() {
        return color;
    }
    
    ofColor color;
    ofImage img;
    bool bTryLoad = false;
	std::string filepath;
};

class ofxSvgCircle : public ofxSvgElement {
public:
    ofxSvgCircle() { type = OFX_SVG_TYPE_CIRCLE; }
    
    float getRadius() {return radius;}
    
    float radius = 10.0;
};

class ofxSvgEllipse : public ofxSvgElement {
public:
    ofxSvgEllipse() { type = OFX_SVG_TYPE_ELLIPSE; }
    
    float radiusX, radiusY = 10.0;
};

class ofxSvgPath : public ofxSvgElement {
public:
    ofxSvgPath() { type = OFX_SVG_TYPE_PATH; }
};


class ofxSvgText : public ofxSvgRectangle {
public:
    
    class Font {
    public:
		std::string fontFamily;
		std::map< int, ofTrueTypeFont > sizes;
		std::map< int, ofTexture > textures;
    };
    
    static std::map< std::string, Font > fonts;
    
    class TextSpan {
    public:
        TextSpan() {
            text = "";
            fontSize = 12;
            lineHeight = 0;
        }
        
		std::string text;
        int fontSize = 12;
		std::string fontFamily;
        ofRectangle rect;
        ofColor color;
        float lineHeight = 12;
        ofTrueTypeFont& getFont();
    };
    
    static bool sortSpanOnFontFamily( const TextSpan& a, const TextSpan& b ) {
        return a.fontFamily < b.fontFamily;
    }
    
    static bool sortSpanOnFontSize( const TextSpan& a, const TextSpan& b ) {
        return a.fontSize < b.fontSize;
    }
    
    ofxSvgText() { type = OFX_SVG_TYPE_TEXT; fdirectory=""; bCentered=false; alpha=1.0; bOverrideColor=false; }
    
    void create();
    void draw();
    
    void setFontDirectory( std::string aPath ) {
        fdirectory = aPath;
//		cout << "Setting the font directory to " << fdirectory << endl;
    }
    
    void overrideColor( ofColor aColor ) {
        bOverrideColor = true;
        _overrideColor = aColor;
    }
    
    ofRectangle getRectangle();
    
	std::map< std::string, std::map<int, ofMesh> > meshes;
	std::vector< TextSpan > textSpans;
    
	std::string fdirectory;
    bool bCentered = false;
    float alpha = 1.;
    ofVec2f ogPos;
    
protected:
    static ofTrueTypeFont defaultFont;
	bool _recursiveFontDirSearch(std::string afile, std::string aFontFamToLookFor, std::string& fontpath);
    ofFloatColor _overrideColor;
    bool bOverrideColor;
};














