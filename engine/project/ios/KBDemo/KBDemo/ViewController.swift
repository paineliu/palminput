//
//  ViewController.swift
//  KBDemo
//
//  Created by liutingchao on 2021/2/11.
//

import UIKit

class ViewController: UIViewController{
    var inputMgrView: InputMgrView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.isMultipleTouchEnabled = true
        
        let screen = UIScreen.main.bounds
        let labelWidth:CGFloat = screen.width;
        let labelHeight:CGFloat = screen.width * 0.7;
        let labelTopView:CGFloat = screen.height - labelHeight
        let frame = CGRect(x: 0, y:labelTopView, width: labelWidth, height: labelHeight)
        
        inputMgrView = InputMgrView(frame:frame)
        self.view.addSubview(inputMgrView)
        
        inputMgrView.show(inputKeyboard: .Chinese)

    }
}

