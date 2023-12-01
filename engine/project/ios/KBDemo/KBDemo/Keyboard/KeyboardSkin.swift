//
//  KeyboardSkin.swift
//  BitKeyboard
//
//  Created by liutingchao on 2021/2/11.
//  Copyright © 2021 Harrison. All rights reserved.
//

import Foundation
import UIKit

struct KeyboardColor: Codable {
    var rgb: Int
    var alpha: Int
    
    @available(iOSApplicationExtension 13.0, *)
    func cgColor() -> CGColor {
        return CGColor.init(red: CGFloat(rgb & 0xFF0000 >> 16) / 255, green: CGFloat(rgb & 0xFF00 >> 8) / 255, blue:CGFloat(rgb & 0xFF) / 255, alpha: CGFloat(100 - alpha) / 100)
    }
}

struct KeyboardFont: Codable {
    var size: Int
    var color: KeyboardColor
}

struct KeyboardBorder: Codable {
    var color: KeyboardColor
    var width: Int
    var rx: Int
    var ry: Int
}

struct KeyboardShadow: Codable {
    var color: KeyboardColor
    var dx: Int
    var dy: Int
    var radius: Int
}

struct KeyboardRect : Codable {
    var left: CGFloat
    var top: CGFloat
    var right: CGFloat
    var bottom: CGFloat
}

struct KeyboardButton : Codable {
    var id: String
    var label: String
    var tag: String?
    var origRect: KeyboardRect
    var dispRect: CGRect?
    var touchRect: KeyboardRect?
    var realRect: CGRect?
    var styleId: Int
}

struct KeyboardButtonStyleState : Codable {
    var bgColor: KeyboardColor
    
    var btnColor: KeyboardColor
    var btnPadding: KeyboardRect
    var btnBorder: KeyboardBorder
    var btnShadow: KeyboardShadow
    var labelFont: KeyboardFont
    var tagFont: KeyboardFont

}

struct KeyboardButtonStyle : Codable {
    var id: String
    
    var normal: KeyboardButtonStyleState
    var press: KeyboardButtonStyleState
}

struct KeyboardSkin : Codable {
    var origRect: KeyboardRect
    var bgImg: String
    var bgColor: KeyboardColor
    var buttonStyles:[KeyboardButtonStyle]
    var buttons:[KeyboardButton]
    var xRadio:CGFloat?
    var yRadio:CGFloat?
    
    public mutating func resize(frame: CGRect){
        self.xRadio = (frame.width) / (self.origRect.right - self.origRect.left);
        self.yRadio = (frame.height) / (self.origRect.bottom - self.origRect.top);
        for index in 0..<buttons.count{
            let rect = buttons[index].origRect
            buttons[index].dispRect = CGRect(x:rect.left * self.xRadio!, y:rect.top * self.yRadio!, width:(rect.right - rect.left) * self.xRadio!, height:(rect.bottom - rect.top) * self.yRadio!);
            if (buttons[index].touchRect != nil) {
                let touchRect = buttons[index].touchRect
                buttons[index].realRect = CGRect(x:touchRect!.left * self.xRadio!, y:touchRect!.top * self.yRadio!, width:(touchRect!.right - touchRect!.left) * self.xRadio!, height:(touchRect!.bottom - touchRect!.top) * self.yRadio!);
            } else {
                buttons[index].realRect = CGRect(x:rect.left * self.xRadio!, y:rect.top * self.yRadio!, width:(rect.right - rect.left) * self.xRadio!, height:(rect.bottom - rect.top) * self.yRadio!);
            }
        }
    }
}
