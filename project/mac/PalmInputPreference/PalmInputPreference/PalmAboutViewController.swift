//
//  PalmAboutViewController.swift
//  PalmInputPreference
//
//  Created by Harrison on 2018/10/9.
//  Copyright © 2018年 Harrison. All rights reserved.
//

import Cocoa

class PalmAboutViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
 
    static func instantiate() -> PalmAboutViewController
    {
        return NSStoryboard(name: "Main", bundle: nil).instantiateController(withIdentifier: "about") as! PalmAboutViewController
    }
}
