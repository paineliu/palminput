//
//  ViewController.swift
//  PalmInputAbout
//
//  Created by Harrison on 2018/10/2.
//  Copyright © 2018 Harrison. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    @IBOutlet weak var labelVer: NSTextField!
    @IBOutlet weak var btn: NSButton!
    @IBAction func installNow(sender: AnyObject) {
        labelVer.stringValue = "dfd"
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

