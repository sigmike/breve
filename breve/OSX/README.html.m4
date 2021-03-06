<HEAD>
<TITLE>the breve simulation environment README</TITLE>
<STYLE TYPE="text/css">
	BODY, P, LI { 
		color: black; 
		background: white; 
		font-family: Geneva,Arial,Helvetica,Swiss,SunSans-Regular;
		font-size: 90%;
	}
	BODY {
		margin-left: 10%;
		margin-right: 10%;
	}
	TT {
		font-size: 80%;
	}
</STYLE>
</HEAD>
<BODY>
<P>
<CENTER>
<H2><B>the <i>breve</I> simulation environment</B></H2>
<P>
<img src="breve_icon.jpg">
<h4>
<a href="http://www.spiderland.org/breve">http://www.spiderland.org/breve</a><br>
OSX release _VERSION_ - _DATE_<br>
<a href="mailto:jk [at] spiderland.org">jk [at] spiderland.org</a>
</h4>
</CENTER>
<P>
<H4><b>introduction</b></H4>
<BLOCKQUOTE>
<P>
<I>breve</I> is a simulation environment which allows users to define the behaviors and interactions of different types of agents in a simulated 3D world and observe the emergent behaviors.  <I>breve</I> is conceptually similar to packages such as Swarm and StarLogo, but simulates objects in a continuous 3D world with continuous time.  <I>breve</I> thus allows simulation involving realistic collision detection and physics.  
<P>
Agent behaviors are written in Python or in a simple scripting language called "steve".  Users can define an agent's behavior by specifying how the agent interacts with the world at each timestep and how the agent reacts to events like collisions.
<P>
<I>breve</I> includes full collision detection and physical simulation of rigid bodies.  See the Walker and Gravity simulations for examples.  
<P>
For a more detailed introduction of the <I>breve</I> environment, refer to the introductory paper at: <A HREF="http://www.spiderland.org/breve">http://www.spiderland.org/breve</a>.
<P>
This release is version _VERSION_.  Many changes have been made from previous releases, so please read the Changes file to find out what's new.
</BLOCKQUOTE>
<P>
<h4><b>getting started</b></h4>
<BLOCKQUOTE>
<P>
The best way to get started and learn about programming in steve is to examine the demos from the demos menu and watch them run.  A very simple demo showing how objects move around the world is "Getting-Started/RandomWalker".  A fun demo showing how collisions can be handled is the "DLA" demo, which shows a decentralized model of fractal growth.  The more complicated "Swarm" demos show different types of models of flocking birds.  The Gravity demo is a simple simulation using physical simulation features, while the Walker and SuperWalker demos is a more complicated example in which a simulated creature learns how to walk. 
<P>
To run a demo, simply select the demo from the "Demo" menu, and push the play button.  Make changes in the code window and restart the demo to see how your changes affect the simulation.
<P>
After trying the demos, you can consult the breve documentation either <a href="http://www.spiderland.org/breve/documentation.php">online<a> or from the application's help menu to learn more about writing simulations in breve.
</BLOCKQUOTE>
<P>
<H4><b>feedback</b></H4>
<BLOCKQUOTE>
<P>
This software is provided with the hope that it will be useful&mdash;the only way for me to make the software more useful is to hear your feedback.  You can send me mail directly at <A HREF="mailto:jk [at] spiderland.org">jk [at] spiderland.org</A>.  Please let me know how you use the software and how it could be improved.
<P>
If you have any cool or useful simulations or classes that should be included with future releases, mail them as well.
<P>
To join in discussions with other <i>breve</i> users, visit the <a href="http://www.spiderland.org/forums">breve discussion forums</a>.
</BLOCKQUOTE>
<P>
<H4><B>license</B></H4>
<BLOCKQUOTE>
<pre>
The breve Simulation Environment                                           
Copyright (C) 2003-2006 Jonathan Klein                        

This program is free software; you can redistribute it and/or modify       
it under the terms of the GNU General Public License as published by       
the Free Software Foundation; either version 2 of the License, or          
(at your option) any later version.                                        

This program is distributed in the hope that it will be useful,            
but WITHOUT ANY WARRANTY; without even the implied warranty of             
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              
GNU General Public License for more details.                               

You should have received a copy of the GNU General Public License          
along with this program; if not, write to the Free Software                
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  
</pre>
</blockquote>
Portions of this software use the <A HREF="http://ode.org">Open Dynamics Engine library</A>, under the 
following license:
<BLOCKQUOTE>
<PRE>
Open Dynamics Engine
Copyright (c) 2001,2002, Russell L. Smith.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

Neither the names of ODE's copyright owner nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</PRE>
</BLOCKQUOTE>
<P>
Portions of this software use the <A HREF="http://www.jclark.com/xml/expat.html">expat library</A>, under the following license:
<BLOCKQUOTE>
<P>
<PRE>
Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
</PRE>
</BLOCKQUOTE>
</BODY>
