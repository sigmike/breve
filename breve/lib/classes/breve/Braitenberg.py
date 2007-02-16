
import breve

class BraitenbergControl( breve.PhysicalControl ):
	'''This class is used for building simple Braitenberg vehicle  simulations.  To create a Braitenberg vehicle simulation,  subclass BraitenbergControl and use the init method to  create OBJECT(BraitenbergLight) and  OBJECT(BraitenbergVehicle) objects.'''

	def __init__( self ):
		breve.PhysicalControl.__init__( self )
		self.cloudTexture = None
		self.floor = None
		self.floorShape = None
		BraitenbergControl.init( self )

	def init( self ):
		''''''

		self.enableLighting()
		self.enableSmoothDrawing()
		self.floorShape = breve.Shape()
		self.floorShape.initWithCube( breve.vector( 200, 200, 200 ) )
		self.floor = breve.Stationary()
		self.floor.register( self.floorShape, breve.vector( 0, 0, 0 ) )
		self.floor.catchShadows()
		self.pointCamera( breve.vector( 0, 0, 0 ), breve.vector( 3, 3, 24 ) )
		self.enableShadows()
		self.enableReflections()
		self.cloudTexture = breve.Image().load( '''images/clouds.png''' )
		self.setBackgroundColor( breve.vector( 0.400000, 0.400000, 0.900000 ) )
		self.setBackgroundTextureImage( self.cloudTexture )


breve.BraitenbergControl = BraitenbergControl

class BraitenbergVehicle( breve.MultiBody ):
	'''This object is used in conjunction with OBJECT(BraitenbergControl) to create simple Braitenberg vehicles.'''

	def __init__( self ):
		breve.MultiBody.__init__( self )
		self.bodyLink = None
		self.bodyShape = None
		self.sensorShape = None
		self.sensors = []
		self.wheelShape = None
		self.wheels = []
		BraitenbergVehicle.init( self )

	def addSensor( self, location ):
		'''Adds a sensor at location on the vehicle.  This method returns the sensor which is created, a OBJECT(BraitenbergSensor).  You'll use the returned object to connect it to the vehicle's wheels.'''

		sensor = breve.BraitenbergSensor()
		sensor.setShape( self.sensorShape )
		joint = breve.RevoluteJoint()
		joint.setRelativeRotation( breve.vector( 0, 0, 1 ), -1.570000 )
		joint.link( self.bodyLink, sensor, breve.vector( 1, 1, 0 ), location, breve.vector( 0, 0, 0 ) )
		joint.setDoubleSpring( 300, 0.010000, -0.010000 )
		self.addDependency( joint )
		self.addDependency( sensor )
		sensor.setColor( breve.vector( 0, 0, 0 ) )
		self.sensors.append( sensor )
		return sensor


	def addWheel( self, location ):
		'''Adds a wheel at location on the vehicle.  This method returns the wheel which is created, a OBJECT(BraitenbergWheel).  You'll use the returned object to connect it to the vehicle's sensors.'''

		wheel = breve.BraitenbergWheel()
		wheel.setShape( self.wheelShape )
		joint = breve.RevoluteJoint()
		joint.setRelativeRotation( breve.vector( 1, 1, 0 ), 1.570800 )
		joint.link( self.bodyLink, wheel, breve.vector( 0, 0, 1 ), location, breve.vector( 0, 0, 0 ) )
		wheel.setET( 0.800000 )
		wheel.setTexture( 0 )
		wheel.setJoint( joint )
		joint.setStrengthLimit( ( joint.getStrengthHardLimit() / 2 ) )
		wheel.setColor( breve.vector( 0.600000, 0.600000, 0.600000 ) )
		wheel.setMu( 100000 )
		self.addDependency( joint )
		self.addDependency( wheel )
		self.wheels.append( wheel )
		return wheel


	def destroy( self ):
		''''''

		del self.sensorShape
		del self.wheelShape
		del self.bodyShape
		breve.MultiBody.destroy()

	def getDensity( self ):
		''''''

		return 1.000000


	def getWheelRadius( self ):
		''''''

		return 0.600000


	def getWheelWidth( self ):
		''''''

		return 0.100000


	def init( self ):
		''''''

		self.bodyShape = breve.Shape()
		self.bodyShape.initWithCube( breve.vector( 4.000000, 4.000000, 3.000000 ) )
		self.wheelShape = breve.Shape()
		self.wheelShape.initWithPolygonDisk( self.getWheelRadius(), 40, self.getWheelWidth() )
		self.sensorShape = breve.Shape()
		self.sensorShape.initWithPolygonCone( 0.200000, 10, 0.500000 )
		self.bodyShape.setDensity( self.getDensity() )
		self.bodyLink = breve.Link()
		self.bodyLink.setShape( self.bodyShape )
		self.bodyLink.setMu( -1.000000 )
		self.bodyLink.setET( 0.800000 )
		self.setRoot( self.bodyLink )
		self.move( breve.vector( 0, 0, 0 ) )
		self.setTextureScale( 1.500000 )


breve.BraitenbergVehicle = BraitenbergVehicle

class BraitenbergHeavyVehicle( breve.BraitenbergVehicle ):
	'''A heavy duty version of OBJECT(BraitenbergVehicle), this vehicle is heavier and harder to control, but more stable at higher  speeds.'''

	def __init__( self ):
		breve.BraitenbergVehicle.__init__( self )

	def getDensity( self ):
		''''''

		return 20.000000


	def getWheelRadius( self ):
		''''''

		return 0.800000


	def getWheelWidth( self ):
		''''''

		return 0.400000



breve.BraitenbergHeavyVehicle = BraitenbergHeavyVehicle

class BraitenbergLight( breve.Mobile ):
	'''A BraitenbergLight is used in conjunction with OBJECT(BraitenbergControl) and OBJECT(BraitenbergVehicle).  It is what the OBJECT(BraitenbergSensor) objects on the BraitenbergVehicle detect. <p> There are no special behaviors associated with the lights--they're  basically just plain OBJECT(Mobile) objects.'''

	def __init__( self ):
		breve.Mobile.__init__( self )
		BraitenbergLight.init( self )

	def init( self ):
		''''''

		self.setShape( breve.Shape().initWithSphere( 0.300000 ) )
		self.setColor( breve.vector( 1, 1, 0 ) )


breve.BraitenbergLight = BraitenbergLight

class BraitenbergWheel( breve.Link ):
	'''A BraitenbergWheel is used in conjunction with OBJECT(BraitenbergVehicle) to build Braitenberg vehicles.  This class is typically not instantiated manually, since OBJECT(BraitenbergVehicle) creates one for you when you add a wheel to the vehicle. <p> <b>NOTE: this class is included as part of the file "Braitenberg.tz".</b>'''

	def __init__( self ):
		breve.Link.__init__( self )
		self.joint = None
		self.naturalVelocity = 0
		self.newVelocity = 0
		self.oldVelocity = 0
		BraitenbergWheel.init( self )

	def activate( self, n ):
		'''Used internally.'''

		self.newVelocity = ( self.newVelocity + n )

	def init( self ):
		''''''

		self.naturalVelocity = 0
		self.newVelocity = 0

	def postIterate( self ):
		''''''

		if ( self.newVelocity >  ):
			self.newVelocity = 

		if ( self.newVelocity == 0 ):
			self.joint.setJointVelocity( self.oldVelocity )
			self.oldVelocity = ( self.oldVelocity * 0.950000 )

		else:
			self.joint.setJointVelocity( self.newVelocity )
			self.oldVelocity = self.newVelocity


		self.newVelocity = self.naturalVelocity

	def setJoint( self, j ):
		'''Used internally.'''

		self.joint = j

	def setNaturalVelocity( self, n ):
		'''Sets the "natural" velocity of this wheel.  The natural velocity is the speed at which the wheel turns in the absence of sensor input.  '''

		self.naturalVelocity = n


breve.BraitenbergWheel = BraitenbergWheel

class BraitenbergSensor( breve.Link ):
	'''A BraitenbergSensor is used in conjunction with OBJECT(BraitenbergVehicle) to build Braitenberg vehicles.  This class is typically not instantiated manually, since OBJECT(BraitenbergVehicle) creates one for you when you add a sensor to the vehicle. <p> <b>NOTE: this class is included as part of the file "Braitenberg.tz".</b>'''

	def __init__( self ):
		breve.Link.__init__( self )
		self.activationMethod = ''
		self.activationObject = None
		self.bias = 0
		self.direction = breve.vector()
		self.sensorAngle = 0
		self.wheels = []
		BraitenbergSensor.init( self )

	def init( self ):
		''''''

		self.bias = 1.000000
		self.direction = breve.vector( 0, 0, 0 )
		self.sensorAngle = 1.600000

	def iterate( self ):
		''''''

		transDir = ( self.getRotation() * self.direction )
		for i in 		breve.allInstances( "breve.BraitenbergLights" ):
			toLight = ( i.getLocation() - self.getLocation() )
			angle = breve.breveInternalFunctionFinder.angle( self, toLight, transDir )
			if ( angle < self.sensorAngle ):
				strength = len( ( self.getLocation() - i.getLocation() ) )
				strength = ( 1.000000 / ( strength * strength ) )
				if ( self.activationMethod and self.activationObject ):
					strength = self.activationObject.callMethod( self.activationMethod, [ strength ] )


				if ( strength > 10 ):
					strength = 10

				total = ( total + strength )
				lights = ( lights + 1 )




		if ( lights != 0 ):
			total = ( total / lights )

		total = ( ( 50 * total ) * self.bias )
		self.wheels.activate( total )

	def link( self, w ):
		'''Associates this sensor with wheel w.'''

		self.wheels.append( w )

	def setActivationMethod( self, m, o ):
		'''This method specifies an activation method for the sensor.  An activation method is a method which takes as input the strength read by the sensor, and as output returns the strength of the  signal which will travel on to the motor. <p> Your activation function should be defined as: <pre> + to <i>activation-function-name</i> with-sensor-strength s (float): </pre> <p> The default activation method is linear, but more complex vehicles may require non-linear activation functions. '''

		self.activationMethod = m
		self.activationObject = o

	def setBias( self, d ):
		'''Sets the "bias" of this sensor.  The default bias is 1, meaning that the sensor has a positive influence on associated wheels with strength 1.  You can change this to any magnitude, positive or negative.'''

		self.bias = d

	def setSensorAngle( self, n ):
		'''Sets the angle in which this sensor can detect light.  The default value of 1.5 means that the sensor can see most of everything in front of it.  Setting the value to be any higher leads to general wackiness, so I don't suggest it.'''

		self.sensorAngle = n


breve.BraitenbergSensor = BraitenbergSensor


