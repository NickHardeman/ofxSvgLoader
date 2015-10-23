//
//  ofxSvgBase.cpp
//
//  Created by Nick Hardeman on 7/31/15.
//

#include "ofxSvgBase.h"

map< string, ofxSvgText::Font > ofxSvgText::fonts;

#pragma mark - ofxSvgBase
//--------------------------------------------------------------
string ofxSvgBase::getTypeAsString() {
    switch (type) {
        case OFX_SVG_TYPE_GROUP:
            return "Group";
            break;
        case OFX_SVG_TYPE_RECTANGLE:
            return "Rectangle";
            break;
        case OFX_SVG_TYPE_IMAGE:
            return "Image";
            break;
        case OFX_SVG_TYPE_ELLIPSE:
            return "Ellipse";
            break;
        case OFX_SVG_TYPE_CIRCLE:
            return "Circle";
            break;
        case OFX_SVG_TYPE_PATH:
            return "Path";
            break;
        case OFX_SVG_TYPE_TEXT:
            return "Text";
            break;
        default:
            break;
    }
    return "Unknown";
}

//--------------------------------------------------------------
string ofxSvgBase::toString( int nlevel ) {
    
    string tstr = "";
    for( int k = 0; k < nlevel; k++ ) {
        tstr += "   ";
    }
    tstr += getTypeAsString() + " - " + getName() + "\n";
    
    return tstr;
}

#pragma mark - ofxSvgText

//--------------------------------------------------------------
void ofxSvgText::create() {
    // now lets sort the text based on meshes that we need to create //
    vector< TextSpan > tspans = textSpans;
    
    map< string, map< int, vector<TextSpan> > > tspanFonts;
    for( int i = 0; i < tspans.size(); i++ ) {
        if( tspanFonts.count( tspans[i].fontFamily ) == 0 ) {
            map< int, vector<TextSpan> > tmapap;
            tspanFonts[ tspans[i].fontFamily ] = tmapap;
        }
        map< int, vector<TextSpan> >& spanMap = tspanFonts[ tspans[i].fontFamily ];
        if( spanMap.count(tspans[i].fontSize) == 0 ) {
            vector< TextSpan > tvec;
            spanMap[ tspans[i].fontSize ] = tvec;
        }
        spanMap[ tspans[i].fontSize ].push_back( tspans[i] );
    }
    
    
    bool bHasFontDirectory = false;
//    cout << "checking directory: " << fdirectory+"/fonts/" << endl;
    string fontsDirectory = "fonts/";
    if( fdirectory != "" ) {
        fontsDirectory = fdirectory+"/fonts/";
    }
    if( ofFile::doesFileExist( fontsDirectory )) {
        bHasFontDirectory = true;
    }
    
    map< string, map< int, vector<TextSpan> > >::iterator mainIt;
    for( mainIt = tspanFonts.begin(); mainIt != tspanFonts.end(); ++mainIt ) {
        if( fonts.count(mainIt->first) == 0 ) {
            Font tafont;
            tafont.fontFamily = mainIt->first;
            fonts[ mainIt->first ] = tafont;
        }
        
        // now create a mesh for the family //
        // map< string, map<int, ofMesh> > meshes;
        if( meshes.count(mainIt->first) == 0 ) {
            map< int, ofMesh > tempMeshMap;
            meshes[ mainIt->first ] = tempMeshMap;
        }
        
        Font& tfont = fonts[ mainIt->first ];
        map< int, ofMesh >& meshMap = meshes[ mainIt->first ];
        
        map< int, vector<TextSpan> >::iterator vIt;
        for( vIt = mainIt->second.begin(); vIt != mainIt->second.end(); ++vIt ) {
            vector<TextSpan>& spanSpans = vIt->second;
            bool bFontLoadOk = true;
            if( tfont.sizes.count( vIt->first ) == 0 ) {
                ofTrueTypeFont datFontTho;
//                bFontLoadOk = datFontTho.loadFont( tfont.fontFamily, vIt->first );
//                string _filename, int _fontSize, bool _bAntiAliased, bool _bFullCharacterSet, bool _makeContours, float _simplifyAmt, int _dpi
                // first let's see if the fonts are provided. Some system fonts are .dfont that have several of the faces
                // in them, but OF isn't setup to parse them, so we need each bold, regular, italic, etc to be a .ttf font //
                string tfontPath = tfont.fontFamily;
                if( bHasFontDirectory ) {
                    ofDirectory tfDir;
                    tfDir.listDir( fontsDirectory );
                    for( int ff = 0; ff < tfDir.size(); ff++ ) {
                        ofFile tfFile = tfDir.getFile(ff);
                        if( tfFile.getExtension() == "ttf" || tfFile.getExtension() == "otf" ) {
                            if( ofToLower(tfFile.getBaseName()) == ofToLower(tfont.fontFamily) ) {
                                ofLogNotice("ofxSvgText found font file for " ) << tfont.fontFamily;
                                tfontPath = tfFile.getAbsolutePath();
                            }
                        }
                    }
                }
                
                bFontLoadOk = datFontTho.loadFont( tfontPath, vIt->first, true, false, false, 0.5, 72 );
                if(bFontLoadOk) {
                    datFontTho.setSpaceSize( 0.57 );
                    tfont.sizes[ vIt->first ]       = datFontTho;
                    tfont.textures[ vIt->first ]    = datFontTho.getFontTexture();
                } else {
                    ofLogError("ofxSvgLoader - error loading font family: ") << tfont.fontFamily << " size: " << vIt->first;
                }
            }
            if( !bFontLoadOk ) continue;
            
            if( meshMap.count(vIt->first) == 0 ) {
                meshMap[ vIt->first ] = ofMesh();
            }
            ofMesh& tmesh = meshMap[ vIt->first ];
            
            ofTrueTypeFont& ttfont = tfont.sizes[ vIt->first ];
            for( int i = 0; i < spanSpans.size(); i++ ) {
                // create a mesh here //
                TextSpan& cspan = spanSpans[i];
//                cout << "font family: " << cspan.fontFamily << " size: " << cspan.fontSize << " text: " << cspan.text << endl;
                
                ofMesh& stringMesh  = ttfont.getStringMesh( cspan.text, cspan.rect.x, cspan.rect.y );
                int offsetIndex     = tmesh.getNumVertices();
                
                vector<ofIndexType>& tsIndices = stringMesh.getIndices();
                for( int k = 0; k < tsIndices.size(); k++ ) {
                    tsIndices[k] = tsIndices[k] + offsetIndex;
                }
                
                ofFloatColor tcolor = cspan.color;
                vector< ofFloatColor > tcolors;
                tcolors.assign( stringMesh.getVertices().size(), tcolor );
                
                tmesh.addIndices( tsIndices );
                tmesh.addVertices( stringMesh.getVertices() );
                tmesh.addTexCoords( stringMesh.getTexCoords() );
                tmesh.addColors( tcolors );
            }
        }
    }
}

//--------------------------------------------------------------
void ofxSvgText::draw() {
//    map< string, map<int, ofMesh> > meshes;
    ofSetColor( 255, 255, 255 );
    map< string, map<int, ofMesh> >::iterator mainIt;
    
    ofPushMatrix(); {
        ofTranslate( pos.x, pos.y );
        if( rotation > 0 ) ofRotateZ( rotation );
        ofTexture* tex;
        for( mainIt = meshes.begin(); mainIt != meshes.end(); ++mainIt ) {
            string fontFam = mainIt->first;
            map< int, ofMesh >::iterator mIt;
            for( mIt = meshes[ fontFam ].begin(); mIt != meshes[ fontFam ].end(); ++mIt ) {
                int fontSize = mIt->first;
                // let's check to make sure that the texture is there, so that we can bind it //
                bool bHasTexture = false;
                // static map< string, Font > fonts;
                if( fonts.count( fontFam ) ) {
                    if( fonts[ fontFam ].textures.count( fontSize ) ) {
                        bHasTexture = true;
                        tex = &fonts[ fontFam ].textures[ fontSize ];
                    }
                }
                
                if( bHasTexture ) tex->bind();
                ofMesh& tMeshMesh = mIt->second;
                tMeshMesh.draw();
                if( bHasTexture ) tex->unbind();
            }
        }
    } ofPopMatrix();
}










