//
//  AKTablePlot.h
//  AudioKit
//
//  Created by Aurelius Prochazka on 2/9/15.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#import "AKTable.h"
#import "AKPlotView.h"

/// Plots the values of the given table
IB_DESIGNABLE
@interface AKTablePlot : AKPlotView

#if TARGET_OS_IPHONE
@property IBInspectable UIColor *lineColor;
#else
@property IBInspectable NSColor *lineColor;
#endif
@property IBInspectable CGFloat lineWidth;

/// Creates the table plot
/// @param frame Bounding frame for the plot
/// @param table Table to plot
- (instancetype)initWithFrame:(CGRect)frame table:(AKTable *)table;

@property (nonatomic) AKTable *table;

@end
