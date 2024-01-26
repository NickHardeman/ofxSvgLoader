//
//  ofxSvgGroup.h
//
//  Created by Nick Hardeman on 7/31/15.
//

#pragma once
#include "ofxSvgBase.h"

class ofxSvgGroup : public ofxSvgBase {
public:
    ofxSvgGroup() { type = OFX_SVG_TYPE_GROUP; }
    
    virtual void draw();
    
	std::vector< std::shared_ptr<ofxSvgBase> >& getElements();
	std::vector< std::shared_ptr<ofxSvgBase> > getAllElements();
    
    template<typename ofxSvgType>
	std::vector< std::shared_ptr<ofxSvgType> > getElementsForType( std::string aPathToGroup="", bool bStrict= false ) {
        
//		std::shared_ptr< ofxSvgType > temp( new ofxSvgType() );
		std::shared_ptr<ofxSvgType> temp = std::make_shared<ofxSvgType>();
        int sType = temp->getType();
        
		std::vector< std::shared_ptr<ofxSvgType> > telements;
        
		std::vector< std::shared_ptr<ofxSvgBase> > elementsToSearch;
        if( aPathToGroup == "" ) {
            elementsToSearch = elements;
        } else {
			std::shared_ptr< ofxSvgBase > temp = getElementForName( aPathToGroup, bStrict );
            if( temp ) {
                if( temp->isGroup() ) {
					std::shared_ptr< ofxSvgGroup > tgroup = std::dynamic_pointer_cast< ofxSvgGroup>( temp );
                    elementsToSearch = tgroup->elements;
                }
            }
        }
        
        if( !elementsToSearch.size() && elements.size() ) {
            elementsToSearch = elements;
        }
        
        for( size_t i = 0; i < elementsToSearch.size(); i++ ) {
            if( elementsToSearch[i]->getType() == sType ) {
                telements.push_back( std::dynamic_pointer_cast< ofxSvgType>(elementsToSearch[i]) );
            }
        }
        return telements;
    }
	
	template<typename ofxSvgType>
	std::vector< std::shared_ptr<ofxSvgType> > getAllElementsForType() {
		
		std::shared_ptr<ofxSvgType> temp = std::make_shared<ofxSvgType>();
		int sType = temp->getType();
		
		std::vector< std::shared_ptr<ofxSvgType> > telements;
		
		auto elementsToSearch = getAllElements();
		
		for( size_t i = 0; i < elementsToSearch.size(); i++ ) {
			if( elementsToSearch[i]->getType() == sType ) {
				telements.push_back( std::dynamic_pointer_cast<ofxSvgType>(elementsToSearch[i]) );
			}
		}
		return telements;
	}
    
	std::shared_ptr< ofxSvgBase > getElementForName( std::string aPath, bool bStrict = false );
    
    template<typename ofxSvgType>
	std::shared_ptr< ofxSvgType > get( std::string aPath, bool bStrict = false ) {
		std::shared_ptr< ofxSvgType > stemp = std::dynamic_pointer_cast< ofxSvgType >( getElementForName( aPath, bStrict ) );
        return stemp;
    }
    
    template<typename ofxSvgType>
	std::shared_ptr< ofxSvgType > get( int aIndex, bool bStrict = false ) {
		std::shared_ptr< ofxSvgType > stemp = std::dynamic_pointer_cast< ofxSvgType >( elements[ aIndex ] );
        return stemp;
    }
    
    bool replace( std::shared_ptr<ofxSvgBase> aOriginal, std::shared_ptr<ofxSvgBase> aNew );
    
    virtual std::string toString(int nlevel = 0);
    
    int getNumChildren();
    
    void disableColors();
    void enableColors();
    
protected:
    void getElementForNameRecursive( std::vector< std::string >& aNamesToFind, std::shared_ptr< ofxSvgBase >& aTarget, std::vector< std::shared_ptr<ofxSvgBase> >& aElements, bool bStrict = false );
    void _getAllElementsRecursive( std::vector< std::shared_ptr< ofxSvgBase > >& aElesToReturn, std::shared_ptr<ofxSvgBase> aele );
    
    void _replaceElementRecursive( std::shared_ptr< ofxSvgBase > aTarget, std::shared_ptr< ofxSvgBase > aNew, std::vector< std::shared_ptr<ofxSvgBase> >& aElements, bool& aBSuccessful );
    
	std::vector< std::shared_ptr<ofxSvgBase> > elements;
};

















