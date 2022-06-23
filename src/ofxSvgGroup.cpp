//
//  ofxSvgGroup.cpp
//
//  Created by Nick Hardeman on 7/31/15.
//

#include "ofxSvgGroup.h"

//--------------------------------------------------------------
void ofxSvgGroup::draw() {
    int numElements = elements.size();
    bool bTrans = pos.x != 0 || pos.y != 0.0;
    if( bTrans ) {
        ofPushMatrix();
        ofTranslate(pos.x, pos.y);
    }
    for( int i = 0; i < numElements; i++ ) {
        elements[i]->draw();
    }
    if( bTrans ) {
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
vector< shared_ptr<ofxSvgBase> >& ofxSvgGroup::getElements() {
    return elements;
}

//--------------------------------------------------------------
vector< shared_ptr<ofxSvgBase> > ofxSvgGroup::getAllElements() {
    vector< shared_ptr< ofxSvgBase > > retElements;
    
    for( auto ele : elements ) {
        _getAllElementsRecursive( retElements, ele );
    }
    
    return retElements;
}

// flattens out hierarchy //
//--------------------------------------------------------------
void ofxSvgGroup::_getAllElementsRecursive( vector< shared_ptr< ofxSvgBase > >& aElesToReturn, shared_ptr<ofxSvgBase> aele ) {
    
    if( aele ) {
        if( aele->isGroup() ) {
            shared_ptr< ofxSvgGroup > tgroup = dynamic_pointer_cast< ofxSvgGroup >(aele);
            for( auto ele : tgroup->getElements() ) {
                _getAllElementsRecursive( aElesToReturn, ele );
            }
        } else {
            aElesToReturn.push_back( aele );
        }
    }
    
}

//--------------------------------------------------------------
shared_ptr< ofxSvgBase > ofxSvgGroup::getElementForName( string aPath, bool bStrict ) {
    
    vector< string > tsearches;
    if( ofIsStringInString( aPath, ":" ) ) {
        tsearches = ofSplitString( aPath, ":" );
    } else {
        tsearches.push_back( aPath );
    }
    
    shared_ptr< ofxSvgBase > temp;
    getElementForNameRecursive( tsearches, temp, elements, bStrict );
    return temp;
}

//--------------------------------------------------------------
void ofxSvgGroup::getElementForNameRecursive( vector< string >& aNamesToFind, shared_ptr< ofxSvgBase >& aTarget, vector< shared_ptr<ofxSvgBase> >& aElements, bool bStrict ) {
    
    for( int i = 0; i < aElements.size(); i++ ) {
        bool bFound = false;
        if(bStrict) {
            if( aElements[i]->getName() == aNamesToFind[0] ) {
                bFound = true;
            }
        } else {
            if( ofIsStringInString( aElements[i]->getName(), aNamesToFind[0] )) {
//                    cout << "Found--- " << aNamesToFind[0] << endl;
                bFound = true;
            }
            
            if (!bFound && aElements[i]->getType() == OFX_SVG_TYPE_TEXT) {
                
                if (aElements[i]->getName() == "No Name") {
                    // the ids for text block in illustrator are weird,
                    // so try to grab the name from the text contents //
                    shared_ptr<ofxSvgText> etext = dynamic_pointer_cast<ofxSvgText>(aElements[i]);
                    if (etext) {
                        if (etext->textSpans.size()) {
//                            cout << "Searching for " << aNamesToFind[0] << " in " << etext->textSpans.front().text << endl;
                            if(ofIsStringInString( etext->textSpans.front().text, aNamesToFind[0] )) {
                                bFound = true;
                            }
                        }
                    }
                }
            }
            
        }
        
        if( bFound == true ) {
            aNamesToFind.erase( aNamesToFind.begin() );
            if( aNamesToFind.size() == 0 || aElements[i]->getType() != OFX_SVG_TYPE_GROUP ) {
                bool bgood = false;
                if( aElements[i] ) {
                    bgood = true;
                }
//                cout << "going to return one of the elements " << aNamesToFind.size() << " good: " << bgood << " " << endl;
                aTarget = aElements[i];
                break;
            } else {
                if( aElements[i]->getType() == OFX_SVG_TYPE_GROUP ) {
                    shared_ptr< ofxSvgGroup > tgroup = dynamic_pointer_cast< ofxSvgGroup >( aElements[i] );
                    getElementForNameRecursive( aNamesToFind, aTarget, tgroup->elements, bStrict );
                    break;
                }
            }
        }
    }
}

//--------------------------------------------------------------
bool ofxSvgGroup::replace( shared_ptr<ofxSvgBase> aOriginal, shared_ptr<ofxSvgBase> aNew ) {
    bool bReplaced = false;
    _replaceElementRecursive( aOriginal, aNew, elements, bReplaced );
    return bReplaced;
}

//--------------------------------------------------------------
void ofxSvgGroup::_replaceElementRecursive( shared_ptr< ofxSvgBase > aTarget, shared_ptr< ofxSvgBase > aNew, vector< shared_ptr<ofxSvgBase> >& aElements, bool& aBSuccessful ) {
    for( int i = 0; i < aElements.size(); i++ ) {
        bool bFound = false;
        if( aTarget == aElements[i] ) {
            bFound = true;
            aBSuccessful = true;
            aElements[i] = aNew;
            break;
        }
        if( !bFound ) {
            if( aElements[i]->getType() == OFX_SVG_TYPE_GROUP ) {
                auto tgroup = dynamic_pointer_cast< ofxSvgGroup >( aElements[i] );
                _replaceElementRecursive(aTarget, aNew, tgroup->elements, aBSuccessful );
            }
        }
    }
}

//--------------------------------------------------------------
string ofxSvgGroup::toString(int nlevel ) {
    
    string tstr = "";
    for( int k = 0; k < nlevel; k++ ) {
        tstr += "   ";
    }
    tstr += getTypeAsString() + " - " + getName() + "\n";
    
    if( elements.size() ) {
        for( int i = 0; i < elements.size(); i++ ) {
            tstr += elements[i]->toString( nlevel+1);
        }
    }
    
    return tstr;
}

//--------------------------------------------------------------
int ofxSvgGroup::getNumChildren() {
    return (int)elements.size();
}

//--------------------------------------------------------------
void ofxSvgGroup::disableColors() {
    auto telements = getAllElements();
    for( auto& ele : telements ) {
        ele->setUseShapeColor(false);
    }
}

//--------------------------------------------------------------
void ofxSvgGroup::enableColors() {
    auto telements = getAllElements();
    for( auto& ele : telements ) {
        ele->setUseShapeColor(true);
    }
}








