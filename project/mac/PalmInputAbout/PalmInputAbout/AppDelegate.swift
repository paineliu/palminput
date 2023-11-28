//
//  AppDelegate.swift
//  PalmInputAbout
//
//  Created by Harrison on 2018/10/2.
//  Copyright © 2018 Harrison. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        NSApp.activate(ignoringOtherApps: true)
        NSApp.windows[0].makeKeyAndOrderFront(self)
    }

    func applicationWillBecomeActive(_ notification: Notification) {
        NSApp.windows[0].makeKeyAndOrderFront(self)
    }
    
    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

