/*****************************************************************************
 *                                                                           *
 * The breve Simulation Environment                                          *
 * Copyright (C) 2000, 2001, 2002, 2003 Jonathan Klein                       *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *****************************************************************************/

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#import "steve.h"
#import "engine.h"

@interface slObjectOutlineItem : NSObject {
    NSString *name;
    brEval eval;
    int offset;
    stInstance *instance;
	stObject *object;

    int childCount;

    BOOL isArray;
    int arrayType;
    int arrayOffset;

    BOOL isList;

    id *childObjects;

    int theIndex;
    brEvalListHead *theEvalList;

}

- (id)initWithEval:(brEval*)e name: (NSString*)name withVar:(stVar*)v withOffset: (int)offset instance:(stInstance*)i;

- (NSString*)getName;
- (NSString*)getValue;
- (void)getEval:(brEval*)eval;
- (void)setInstance:(stInstance*)i;

- (void)updateChildCount:(int)newChildCount;

- (void)setList:(brEvalListHead*)e index:(int)index;

- (int)getChildCount;
- (BOOL)getExpandable;

- (id)childAtIndex:(int)index;

@end