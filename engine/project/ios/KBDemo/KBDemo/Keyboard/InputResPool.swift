//
//  InputResPool.swift
//  KBDemo
//
//  Created by liutingchao on 2021/3/25.
//

import Foundation
import UIKit

class InputResPool: NSObject {
    var imageDict = [String: UIImage]()
    var _skinPath:String!
    
    init(skinPath:String) {
        super.init()
        _skinPath = skinPath
    }
    
    func getImage(filename:String) -> UIImage?{

        if let fullname = Bundle.main.path(forResource: filename, ofType: "png", inDirectory:_skinPath){
            if let image = imageDict[filename] {
                return image
            }
            else {
                imageDict[filename] = UIImage(contentsOfFile: fullname)
                return imageDict[filename]
            }
        }
        
        return nil
    }
}
