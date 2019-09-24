//
//  ofxSvgLoader.cpp
//
//  Created by Nick Hardeman on 7/31/15.
//

#include "ofxSvgLoader.h"
extern "C"{
#include "svgtiny.h"
}

//--------------------------------------------------------------
ofxSvgLoader::ofxSvgLoader() {
    svgPath     = "";
    folderPath  = "";
    fontsDirectory  = "";
}

//--------------------------------------------------------------
bool ofxSvgLoader::load( string aPathToSvg ) {
    elements.clear();
    
    ofFile mainXmlFile( aPathToSvg, ofFile::ReadOnly );
    ofBuffer tMainXmlBuffer( mainXmlFile );
    
    svgPath     = aPathToSvg;
    folderPath  = ofFilePath::getEnclosingDirectory( aPathToSvg, false );
    
    ofXml xml;
    if( !xml.load(tMainXmlBuffer )) {
        return false;
    }
    
    
    if( xml ) {
//        Poco::XML::Element *svgNode     = document->documentElement();
//        Poco::XML::Attr* viewBoxNode = svgNode->getAttributeNode("viewbox");
        
        ofXml svgNode = xml.getFirstChild();
        ofXml::Attribute viewBoxAttr = svgNode.getAttribute("viewBox");
        if(svgNode) {
            bounds.x        = ofToFloat( cleanString( svgNode.getAttribute("x").getValue(), "px") );
            bounds.y        = ofToFloat( cleanString( svgNode.getAttribute("y").getValue(), "px" ));
            bounds.width    = ofToFloat( cleanString( svgNode.getAttribute("width").getValue(), "px" ));
            bounds.height   = ofToFloat( cleanString( svgNode.getAttribute("height").getValue(), "px" ));
            viewbox = bounds;
        }
        
        if( viewBoxAttr ) {
            string tboxstr = viewBoxAttr.getValue();
            vector< string > tvals = ofSplitString( tboxstr, " " );
            if( tvals.size() == 4 ) {
                viewbox.x = ofToFloat(tvals[0] );
                viewbox.y = ofToFloat( tvals[1] );
                viewbox.width = ofToFloat( tvals[2] );
                viewbox.height = ofToFloat( tvals[3] );
            }
        }
        
        // everything in ofXml is a deep copy, so we have to keep loading xml
        // pass in a root node so that we can later pass to tiny svg //
        ofXml nxml;
        nxml.load(tMainXmlBuffer);
        ofXml cleanRootSvgNode = nxml.getFirstChild();
        auto lchild = cleanRootSvgNode.getLastChild();
        int numTries = 0;
        while( lchild && numTries < 100000 ) {
//            if( lchild ) cout << "Removing child: " << lchild.getName() << " id: " << lchild.getAttribute("id").getValue() << endl;
            cleanRootSvgNode.removeChild(lchild);
            lchild = cleanRootSvgNode.getLastChild();
            numTries++;
        }
        
        parseXmlNode( cleanRootSvgNode, svgNode, elements );
        
//        document->release();
    }
    
//    Poco::XML::DOMParser parser;
//    Poco::XML::Document* document;
//
//    try {
//        document = parser.parseMemory( tMainXmlBuffer.getData(), tMainXmlBuffer.size() );
//        document->normalize();
//    } catch( exception e ) {
//        short msg = atoi(e.what());
//        ofLogError() << "loadFromBuffer " << msg << endl;
//        if( document ) {
//            document->release();
//        }
//        return false;
//    }
    
//
//    if( document ) {
//        Poco::XML::Element *svgNode     = document->documentElement();
//
//        Poco::XML::Attr* viewBoxNode = svgNode->getAttributeNode("viewbox");
//
//        bounds.x        = ofToFloat( cleanString( svgNode->getAttribute("x"), "px") );
//        bounds.y        = ofToFloat( cleanString( svgNode->getAttribute("y"), "px" ));
//        bounds.width    = ofToFloat( cleanString( svgNode->getAttribute("width"), "px" ));
//        bounds.height   = ofToFloat( cleanString( svgNode->getAttribute("height"), "px" ));
//        viewbox = bounds;
//
//        if( viewBoxNode ) {
//            string tboxstr = viewBoxNode->getNodeValue();
//            vector< string > tvals = ofSplitString( tboxstr, " " );
//            if( tvals.size() == 4 ) {
//                viewbox.x = ofToFloat(tvals[0] );
//                viewbox.y = ofToFloat( tvals[1] );
//                viewbox.width = ofToFloat( tvals[2] );
//                viewbox.height = ofToFloat( tvals[3] );
//            }
//        }
//
//        parseXmlNode( document, svgNode, elements );
//
//        document->release();
//    }
    
    return true;
}

//--------------------------------------------------------------
bool ofxSvgLoader::reload() {
    if( svgPath == "" ) {
        ofLogError("ofxSvgLoader svg path is empty, please call load with file path before calling reload");
        return false;
    }
    return load( svgPath );
}

//--------------------------------------------------------------
void ofxSvgLoader::setFontsDirectory( string aDir ) {
    fontsDirectory = aDir;
    if( fontsDirectory.back() != '/' ) {
        fontsDirectory += '/';
    }
}

//--------------------------------------------------------------
string ofxSvgLoader::toString(int nlevel) {
    
    string tstr = "";
    if( elements.size() ) {
        for( int i = 0; i < elements.size(); i++ ) {
            tstr += elements[i]->toString( nlevel );
        }
    }
    
    return tstr;
}

//--------------------------------------------------------------
string ofxSvgLoader::cleanString( string aStr, string aReplace ) {
    ofStringReplace( aStr, aReplace, "");
    return aStr;
}

//--------------------------------------------------------------
void ofxSvgLoader::parseXmlNode( ofXml& aRootNode, ofXml& aParentNode, vector< shared_ptr<ofxSvgBase> >& aElements ) {
    
    auto kids = aParentNode.getChildren();
    for( auto& kid : kids ) {
        if( kid.getName() == "g" ) {
            auto fkid = kid.getFirstChild();
            if( fkid ) {
                auto tgroup = make_shared<ofxSvgGroup>();
                auto idattr = kid.getAttribute("id");
                if( idattr ) {
                    tgroup->name = idattr.getValue();
                }
                aElements.push_back( tgroup );
                parseXmlNode( aRootNode, kid, tgroup->getElements() );
            }
        } else {
            
            bool bAddOk = addElementFromXmlNode( aRootNode, kid, aElements );
//            ofXml txml;// = aRootNode;
//            txml.appendChild(aRootNode);
            //            txml.appendChild(kid);
//            cout << "----------------------------------" << endl;
//            cout << kid.getName() << " kid: " << kid.getAttribute("id").getValue() << " out xml: " << txml.toString() << endl;
        }
    }
    
//    if( aParentNode->hasChildNodes() ) {
//        Poco::XML::NodeList *list = aParentNode->childNodes();
//        for(int i=0; i < (int)list->length(); i++) {
//            if(list->item(i) && list->item(i)->nodeType() == Poco::XML::Node::ELEMENT_NODE) {
//                Poco::XML::Node* tnode  = (Poco::XML::Element*) list->item(i);
//
//                if( tnode->hasChildNodes() && tnode->nodeName() == "g" ) {
//                    shared_ptr< ofxSvgGroup > tgroup( new ofxSvgGroup() );
//                    aElements.push_back( tgroup );
//                    string attributePath = "[@id]";
//                    Poco::XML::Node *atNode = tnode->getNodeByPath(attributePath);
//                    if( atNode ) {
//                        tgroup->name = atNode->getNodeValue();
//                    }
//
//                    parseXmlNode( document, tnode, tgroup->getElements() );
////                    }
//                } else {
//                    bool bAddOk = addElementFromXmlNode( document, (Poco::XML::Element*)tnode, aElements );
//                }
//            }
//        }
//        list->release();
//    }
}

//--------------------------------------------------------------
bool ofxSvgLoader::addElementFromXmlNode( ofXml& aRootNode, ofXml& tnode, vector< shared_ptr<ofxSvgBase> >& aElements ) {
    shared_ptr< ofxSvgElement > telement;
    
    if( tnode.getName() == "image" ) {
//        telement = make_shared<ofxSvgImage>();
        auto image = make_shared< ofxSvgImage>();
        auto wattr = tnode.getAttribute("width");
        if(wattr) image->rectangle.width  = wattr.getFloatValue();
        auto hattr = tnode.getAttribute("height");
        if(hattr) image->rectangle.height = hattr.getFloatValue();
        auto xlinkAttr = tnode.getAttribute("xlink:href");
        if( xlinkAttr ) {
            image->filepath = folderPath+xlinkAttr.getValue();
//            cout << "image->path = " << image->getFilePath() << endl;
        }
        telement = image;
        
    } else if( tnode.getName() == "ellipse" ) {
//        telement = make_shared<ofxSvgEllipse>();
        auto ellipse = make_shared<ofxSvgEllipse>();
        auto cxAttr = tnode.getAttribute("cx");
        if(cxAttr) ellipse->pos.x = cxAttr.getFloatValue();
        auto cyAttr = tnode.getAttribute("cy");
        if(cyAttr) ellipse->pos.y = cyAttr.getFloatValue();
        
//        shared_ptr< ofxSvgEllipse > ellipse = static_pointer_cast< ofxSvgEllipse>( telement );
        auto rxAttr = tnode.getAttribute( "rx" );
        if(rxAttr) ellipse->radiusX = rxAttr.getFloatValue();
        auto ryAttr = tnode.getAttribute( "ry" );
        if(ryAttr) ellipse->radiusY = ryAttr.getFloatValue();
        
        telement = ellipse;
        
        parseWithSvgTiny( aRootNode, tnode, telement );
        
    } else if( tnode.getName() == "circle" ) {
//        telement = shared_ptr< ofxSvgCircle >( new ofxSvgCircle() );
        auto ellipse = make_shared<ofxSvgCircle>();
        auto cxAttr = tnode.getAttribute("cx");
        if(cxAttr) ellipse->pos.x = cxAttr.getFloatValue();
        auto cyAttr = tnode.getAttribute("cy");
        if(cyAttr) ellipse->pos.y = cyAttr.getFloatValue();
        
        auto rAttr = tnode.getAttribute( "r" );
        if(rAttr) ellipse->radius = rAttr.getFloatValue();
        
//        shared_ptr< ofxSvgCircle > ellipse = static_pointer_cast< ofxSvgCircle>( telement );
//        ellipse->radius = ofToFloat( tnode->getAttribute( "r" ));
        
        telement = ellipse;
        
        parseWithSvgTiny( aRootNode, tnode, telement );
        
    } else if( tnode.getName() == "path" || tnode.getName() == "line" || tnode.getName() == "polyline" || tnode.getName() == "polygon" ) {
        telement = make_shared<ofxSvgPath>();
        parseWithSvgTiny( aRootNode, tnode, telement );
    } else if( tnode.getName() == "rect" ) {
//        telement = shared_ptr< ofxSvgRectangle >( new ofxSvgRectangle() );
        
        auto rect = make_shared<ofxSvgRectangle>();
//        shared_ptr< ofxSvgRectangle > rect = dynamic_pointer_cast< ofxSvgRectangle>( telement );
        auto xattr = tnode.getAttribute("x");
        if(xattr) rect->rectangle.x       = xattr.getFloatValue();
        auto yattr = tnode.getAttribute("y");
        if(yattr) rect->rectangle.y       = yattr.getFloatValue();
        auto wattr = tnode.getAttribute("width");
        if(wattr) rect->rectangle.width   = wattr.getFloatValue();
        auto hattr = tnode.getAttribute("height");
        if(hattr) rect->rectangle.height  = hattr.getFloatValue();
        rect->pos.x = rect->rectangle.x;
        rect->pos.y = rect->rectangle.y;
        
        telement = rect;
        
        parseWithSvgTiny( aRootNode, tnode, telement );
        
        // this shouldn't be drawn at all, may be a rect that for some reason is generated
        // by text blocks //
        if( !rect->isFilled() && !rect->hasStroke() ) {
            telement.reset();
        }
        
    } else if( tnode.getName() == "text" ) {
//        telement = shared_ptr< ofxSvgText >( new ofxSvgText() );
//        shared_ptr< ofxSvgText > text = dynamic_pointer_cast< ofxSvgText>( telement );
        auto text = make_shared<ofxSvgText>();
        telement = text;
//        cout << "has kids: " << tnode->hasChildNodes() << " node value: " << tnode->innerText() << endl;
        if( tnode.getFirstChild() ) {
            
            auto kids = tnode.getChildren();
            for( auto& kid : kids ) {
                if(kid) {
                    if( kid.getName() == "tspan" ) {
                        text->textSpans.push_back( getTextSpanFromXmlNode( kid ) );
                    }
                }
            }
            
//            Poco::XML::NodeList *list = tnode->childNodes();
//            for(int i=0; i < (int)list->length(); i++) {
//                if(list->item(i) && list->item(i)->nodeType() == Poco::XML::Node::ELEMENT_NODE) {
//                    Poco::XML::Element* babyNode  = (Poco::XML::Element*) list->item(i);
//                    if( babyNode->nodeName() == "tspan" ) {
//                        text->textSpans.push_back( getTextSpanFromXmlNode( babyNode ) );
//                    }
//                }
//            }
//            list->release();
            
            // this may not be a text block or it may have to text //
            if( text->textSpans.size() == 0 ) {
                auto ffatrr = tnode.getAttribute("font-family");
                if( ffatrr ) {
//                    cout << "Trying to add in a text span " << tnode->innerText() << endl;
                    text->textSpans.push_back( getTextSpanFromXmlNode( tnode ) );
                }
            }
            
        }
        
        string tempFolderPath = folderPath;
        if( tempFolderPath.back() != '/' ) {
            tempFolderPath += '/';
        }
        if( ofDirectory::doesDirectoryExist( tempFolderPath+"fonts/" )) {
            text->setFontDirectory( tempFolderPath+"fonts/" );
        }
        if( fontsDirectory != "" ) {
            if( ofDirectory::doesDirectoryExist(fontsDirectory)) {
                text->setFontDirectory( fontsDirectory );
            }
        }
        
    } else if( tnode.getName() == "g" ) {
//        if( tnode->hasChildNodes() ) {
//            telement = shared_ptr< ofxSvgGroup >( new ofxSvgGroup() );
//        }
    }
    
    if( !telement ) {
        return false;
    }
    
    if( telement->getType() == ofxSvgBase::OFX_SVG_TYPE_RECTANGLE || telement->getType() == ofxSvgBase::OFX_SVG_TYPE_IMAGE || telement->getType() == OFX_SVG_TYPE_TEXT ) {
        auto transAttr = tnode.getAttribute("transform");
        if( transAttr ) {
            getTransformFromSvgMatrix( transAttr.getValue(), telement->pos, telement->scale.x, telement->scale.y, telement->rotation );
        }
        if( telement->getType() == ofxSvgBase::OFX_SVG_TYPE_IMAGE ) {
            shared_ptr< ofxSvgImage > timg = dynamic_pointer_cast<ofxSvgImage>( telement );
            timg->rectangle.x = timg->pos.x;
            timg->rectangle.y = timg->pos.y;
        }
    }
    
    if( telement->getType() == ofxSvgBase::OFX_SVG_TYPE_TEXT ) {
        shared_ptr< ofxSvgText > text = dynamic_pointer_cast< ofxSvgText>( telement );
        text->ogPos = text->pos;
        text->create();
    }
    
    auto idAttr = tnode.getAttribute("id");
    if( idAttr ) {
        telement->name = idAttr.getValue();
    }
    
    auto disAttr = tnode.getAttribute("display");
    if( disAttr ) {
        if( ofToLower(disAttr.getValue()) == "none" ) {
            telement->setVisible( false );
        }
    }
    
//    if( tnode->hasAttributes() ) {
//        string attributePath = "[@id]";
//        Poco::XML::Node *atNode = tnode->getNodeByPath(attributePath);
//        if( atNode ) {
//            telement->name = atNode->getNodeValue();
//        }
//
//        string displayPath = "[@display]";
//        Poco::XML::Node *displayNode = tnode->getNodeByPath(displayPath);
//        if( displayNode ) {
//            //            telement->name = atNode->getNodeValue();
//            if( displayNode->getNodeValue() == "none" ) {
//                telement->setVisible( false );
//            }
//        }
//    }
    
//    cout << "name: " << telement->name << " type: " << telement->getTypeAsString() << endl;
    
    
    aElements.push_back( telement );
    return true;
}

//--------------------------------------------------------------
void ofxSvgLoader::parseWithSvgTiny( ofXml& aRootNode, ofXml& tnode, shared_ptr<ofxSvgElement> aElement ) {
    
    
    
    // make a lil xml to load for svg tiny //
    ofXml txml;// = aRootNode;
    txml.appendChild(aRootNode);
    auto skid = txml.getChild("svg");
    if( skid ) {
        skid.appendChild( tnode );
    }
    
    if( !txml ) {
        ofLogError("ofxSvgLoader::parseWithSvgTiny parse ERROR!");
        return;
    }
    
    struct svgtiny_diagram * diagram = svgtiny_create();
    ofBuffer tbuff;
    tbuff.set( txml.toString() );
    size_t size = tbuff.size();
    svgtiny_code code = svgtiny_parse(diagram, tbuff.getText().c_str(), size, svgPath.c_str(), 0, 0);
    
//    Poco::XML::Element *path;
//    struct svgtiny_parse_state state;
    aElement->path.clear();
    
    // -- taken from ofxSVG -- ///
    if(code != svgtiny_OK) {
        string msg;
        switch(code){
            case svgtiny_OUT_OF_MEMORY:
                msg = "svgtiny_OUT_OF_MEMORY";
                break;
                
                /*case svgtiny_LIBXML_ERROR:
                 msg = "svgtiny_LIBXML_ERROR";
                 break;*/
                
            case svgtiny_NOT_SVG:
                msg = "svgtiny_NOT_SVG";
                break;
                
            case svgtiny_SVG_ERROR:
                msg = "svgtiny_SVG_ERROR: line " + ofToString(diagram->error_line) + ": " + diagram->error_message;
                break;
                
            default:
                msg = "unknown svgtiny_code " + ofToString(code);
                break;
        }
        ofLogError("ofxSVG") << "load(): couldn't parse \"" << svgPath << "\": " << msg;
    }
    
    //////////////////////////////
    
//    if( aElement->getType() == ofxSvgBase::OFX_SVG_TYPE_PATH ) {
//        if( tnode->nodeName() == "line" ) {
//            svgtiny_parseLine( document, tnode, diagram );
//        } else if( tnode->nodeName() == "path" ) {
//            svgtiny_parsePath( document, tnode, diagram );
//        } else if( tnode->nodeName() == "polyline" ) {
//            svgtiny_parsePolyline( document, tnode, diagram );
//        } else if( tnode->nodeName() == "polygon" ) {
//            svgtiny_parsePolygon( document, tnode, diagram );
//        }
//    } else if( aElement->getType() == ofxSvgBase::OFX_SVG_TYPE_ELLIPSE ) {
//        svgtiny_parseEllipse( document, tnode, diagram );
//    } else if( aElement->getType() == ofxSvgBase::OFX_SVG_TYPE_CIRCLE ) {
//        svgtiny_parseCircle( document, tnode, diagram );
//    } else if( aElement->getType() == ofxSvgBase::OFX_SVG_TYPE_RECTANGLE ) {
//        svgtiny_parseRectangle( document, tnode, diagram );
//    }
    
    if( code == svgtiny_OK ) {
        for(int i = 0; i < (int)diagram->shape_count; i++) {
            if( diagram->shape[i].path ) {
    //            cout << ">>>>>>> path " << i << " " << (int)diagram->shape[i].path_length << endl;
                setupShape( &diagram->shape[i], aElement->path );
                
    //            cout << " path length  = " << aElement->paths.back().getOutline().size() << endl;
            }
        }
    }
    
    svgtiny_free( diagram );
}

// taken from ofxSvg //
void ofxSvgLoader::setupShape( struct svgtiny_shape* shape, ofPath& path ) {
	float * p = shape->path;
    
	path.setFilled(false);
    path.setStrokeWidth( 0 );
    
	if(shape->fill != svgtiny_TRANSPARENT){
		path.setFilled(true);
		path.setFillHexColor(shape->fill);
//		path.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
    }
    
	if(shape->stroke != svgtiny_TRANSPARENT){
		path.setStrokeWidth(shape->stroke_width);
		path.setStrokeHexColor(shape->stroke);
	}
    
	for(int i = 0; i < (int)shape->path_length;){
		if(p[i] == svgtiny_PATH_MOVE){
			path.moveTo(p[i + 1], p[i + 2]);
			i += 3;
		}
		else if(p[i] == svgtiny_PATH_CLOSE){
			path.close();
            
			i += 1;
		}
		else if(p[i] == svgtiny_PATH_LINE){
			path.lineTo(p[i + 1], p[i + 2]);
			i += 3;
		}
		else if(p[i] == svgtiny_PATH_BEZIER){
			path.bezierTo(p[i + 1], p[i + 2],
                          p[i + 3], p[i + 4],
                          p[i + 5], p[i + 6]);
			i += 7;
		}
		else{
			ofLogError("ofxSVG") << "setupShape(): SVG parse error";
			i += 1;
		}
//        cout << "parse path " << i << endl;
	}
    
    
}

//--------------------------------------------------------------
bool ofxSvgLoader::getTransformFromSvgMatrix( string matrix, ofVec2f& apos, float & scaleX, float & scaleY, float & arotation ){
    
    scaleX = 1.0;
    scaleY = 1.0;
    arotation = 0.0;
    
    ofStringReplace(matrix, "matrix(", "");
    ofStringReplace(matrix, ")", "");
    vector <string> matrixNum = ofSplitString(matrix, " ", false, true);
    vector <float> matrixF;
    for(int i = 0; i < matrixNum.size(); i++){
        matrixF.push_back(ofToFloat(matrixNum[i]));
        //cout << " matrix[" << i << "] = " << matrixF[i] << " string version is " << matrixNum[i] << endl;
    }
    
    if( matrixNum.size() == 6 ) {
        
        apos.x = matrixF[4];
        apos.y = matrixF[5];
        
        scaleX = sqrt(matrixF[0] * matrixF[0] + matrixF[1] * matrixF[1]) * (float)ofSign(matrixF[0]);
        scaleY = sqrt(matrixF[2] * matrixF[2] + matrixF[3] * matrixF[3]) * (float)ofSign(matrixF[3]);
        
        arotation = atan(matrixF[2]/matrixF[3]) * RAD_TO_DEG;
        arotation *= -1.0;
        
        //cout << " rotation is " << rotation << endl;
        
        return true;
    }
    return false;
}

//--------------------------------------------------------------
ofColor ofxSvgLoader::getColorFromXmlAttr( string aAtt ) {
    ofStringReplace( aAtt, "#", "" );
    aAtt = "0x"+aAtt;
    ofColor tcolor;
    tcolor.setHex( ofHexToInt( aAtt ) );
    return tcolor;
}

//--------------------------------------------------------------
ofxSvgText::TextSpan ofxSvgLoader::getTextSpanFromXmlNode( ofXml& anode ) {
    ofxSvgText::TextSpan tspan;
    
    string tText = anode.getValue();//aNode->innerText();
    string tFontFam = "Arial";
    auto ffattr = anode.getAttribute("font-family");
    if( ffattr ) {
        tFontFam = ffattr.getValue();
        ofStringReplace( tFontFam, "'", "" );
    }
    int tFontSize = 18;
    auto fsattr = anode.getAttribute("font-size");
    if( fsattr) {
        tFontSize = fsattr.getIntValue();
    }
    float tx = 0;
    auto txattr = anode.getAttribute("x");
    if( txattr) {
        tx = txattr.getFloatValue();
    }
    float ty = 0;
    auto tyattr = anode.getAttribute("y");
    if( tyattr ) {
        ty = tyattr.getFloatValue();
    }
    ofColor tcolor;
    auto fillAttr = anode.getAttribute("fill");
    if( fillAttr ) {
        tcolor = getColorFromXmlAttr( fillAttr.getValue() );
    }
    
    // try to figure out the transform //
//    auto transAttr = anode.getAttribute("transform");
//    if( transAttr ) {
//        // we just need the rect.x and rect.y
//        ofVec2f tpos; float tscalex, tscaley, trotation;
//        getTransformFromSvgMatrix( transAttr.getValue(), tpos, tscalex, tscaley, trotation );
//        tx = tpos.x;
//        ty = tpos.y;
//    }
    
    tspan.text          = tText;
    tspan.fontFamily    = tFontFam;
    tspan.fontSize      = tFontSize;
    tspan.rect.x        = tx;
    tspan.rect.y        = ty;
    tspan.color         = tcolor;
    
    return tspan;
}

//--------------------------------------------------------------
const ofRectangle ofxSvgLoader::getBounds(){
	return bounds;
}

//--------------------------------------------------------------
const ofRectangle ofxSvgLoader::getViewbox(){
	return viewbox;
}
