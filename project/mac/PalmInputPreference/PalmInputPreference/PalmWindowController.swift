//
//  PalmWindowController.swift
//  PalmInputPreference
//
//  Created by Harrison on 2018/10/9.
//  Copyright © 2018年 Harrison. All rights reserved.
//

import Cocoa

class PalmWindowController: NSWindowController {

    var settingViewController:PalmSettingViewController!
    var aboutViewController:PalmAboutViewController!

    
    @IBAction func toolbarActionA(sender: AnyObject) {
        print("toolbarActionA")
        if (settingViewController == nil) {
            settingViewController = PalmSettingViewController.instantiate()
        }
        self.contentViewController = settingViewController
    }
    
    @IBAction func toolbarActionB(sender: AnyObject) {
        print("toolbarActionB")
        if (aboutViewController == nil) {
            aboutViewController = PalmAboutViewController.instantiate()
        }
        self.contentViewController = aboutViewController
    }
    

    
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

}
