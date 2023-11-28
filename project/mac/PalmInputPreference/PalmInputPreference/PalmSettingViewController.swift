//
//  PalmSettingViewController.swift
//  PalmInputPreference
//
//  Created by Harrison on 2018/10/9.
//  Copyright © 2018年 Harrison. All rights reserved.
//

import Cocoa

class PalmSettingViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
    
    static func instantiate() -> PalmSettingViewController
    {
        return NSStoryboard(name: "Main", bundle: nil).instantiateController(withIdentifier: "setting") as! PalmSettingViewController
    }
}
