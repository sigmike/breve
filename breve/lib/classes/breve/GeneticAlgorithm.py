
import breve

class GeneticAlgorithm( breve.Object ):
	'''The GeneticAlgorithm class allows you to set up a genetic algorithm,  in which strategies can be evolved to solve a wide variety of problems. <P> To run a genetic algorithm, you will need to a create custom subclass of this object, GeneticAlgorithm, and of its companion object  OBJECT(GeneticProgrammingIndividual). <P> Your subclass of GeneticAlgorithm must implement two important methods:	 METHOD(start-fitness-test) and METHOD(end-fitness-test).  These methods are automatically called at the beginning and end of the fitness tests	 respectively.'''

	def __init__( self ):
		breve.Object.__init__( self )
		self.bestFitness = 0
		self.crossoverPercent = 0
		self.currentIndividual = 0
		self.currentPopulation = 0
		self.generation = 0
		self.generationLimit = 0
		self.individualClass = ''
		self.migrationHosts = []
		self.migrationPercent = 0
		self.migrationPort = 0
		self.migrationServer = None
		self.mutationPercent = 0
		self.populationSize = 0
		self.populations = []
		self.randomizePercent = 0
		self.seed = None
		self.spatialRadius = 0
		self.testDuration = 0
		self.tournamentSize = 0
		self.transferIn = None
		self.transferOut = None
		GeneticAlgorithm.init( self )

	def acceptUpload( self, migrants, other ):
		''''''

		del self.transferIn
		self.transferIn = migrants

	def dearchive( self ):
		''''''

		self.currentIndividual = ( self.currentIndividual - 1 )
		self.switchIndividual()
		return 1


	def endFitnessTest( self, individual ):
		'''In this method, you should perform any cleanup neccessary when the  fitness test ends.  You should also make sure that the individual's fitness value is set using  OBJECTMETHOD(GeneticAlgorithmIndividual:set-fitness).'''


	def examine( self, currentIndividual ):
		'''Called automatically during each generation's report, this method examines and outputs information about an individual in the population.  The default implementation simply prints out the object variable, but you may override  this method to print out more information about the individual.'''

		print currentIndividual

	def generatePopulation( self ):
		''''''

		if self.populations[ 0 ]:
			del self.populations[ 0 ]

		if self.populations[ 1 ]:
			del self.populations[ 1 ]

		if ( self.individualClass == '''''' ):
			raise Exception( '''A GeneticAlgorithmIndividual subclass must be specified using the method "set-individual-class"''' )


		self.populations = [ [], [] ]
		
		if self.seed:
			self.populations[ 0 ][ 0 ].copy( self.seed )

		self.currentIndividual = -1
		self.switchIndividual()

	def getBestFitness( self ):
		'''Returns the best fitness seen so far.'''

		return self.bestFitness


	def getCrossoverPercent( self ):
		'''Returns the probability of crossover during reproduction.'''

		return self.crossoverPercent


	def getCurrentIndividual( self ):
		'''Returns the current GeneticAlgorithmIndividual subclass instance being evaluated. See also METHOD(get-current-individual-index).'''

		if ( len( self.populations[ self.currentPopulation ] ) == 0 ):
			self.generatePopulation()

		if ( self.currentIndividual == -1 ):
			return 0


		return self.populations[ self.currentPopulation ][ self.currentIndividual ]


	def getCurrentIndividualIndex( self ):
		'''Returns the index of the individual currently being tested.  Returns -1 before any fitness tests have been run. See also METHOD(get-current-individual).'''

		return self.currentIndividual


	def getFitnessTestDuration( self ):
		'''Returns the duration of a single fitness test.  This value can be set with the method METHOD(set-test-duration).  The default value is 20.'''

		return self.testDuration


	def getGeneration( self ):
		'''Returns the current generation.  The generation count begins at 0.'''

		return self.generation


	def getGenerationLimit( self ):
		'''Returns the generation limit.  See METHOD(get-generation-limit) for  details.'''

		return self.generationLimit


	def getMigrationPercent( self ):
		''''''

		return self.migrationPercent


	def getMutationPercent( self ):
		'''Returns the percent probability of mutation during reproduction. The default value is 75%.'''

		return self.mutationPercent


	def getPopulationSize( self ):
		'''Returns the size of the population.  The default value is 30, and can be set with METHOD(set-population-size).'''

		return self.populationSize


	def getRandomizePercent( self ):
		'''Returns the percent probability of a new random individual during reproduction. The default value is 5%.'''

		return self.randomizePercent


	def getSpatialRadius( self ):
		''''''

		return self.spatialRadius


	def getTournamentSize( self ):
		'''Returns the tournament size.  The tournament size specifies how many individuals are considered in a single selection "tournament". The larger the tournament size, the more competative selection becomes. The tournament size can be set with METHOD(set-tournament-size). The default value is 5.  '''

		return self.tournamentSize


	def init( self ):
		''''''

		self.testDuration = 20
		self.populationSize = 30
		self.currentIndividual = 0
		self.mutationPercent = 75
		self.migrationPercent = 0
		self.crossoverPercent = 0
		self.randomizePercent = 5
		self.tournamentSize = 5
		self.spatialRadius = 0
		self.migrationPort = 
		self.populations = [ [], [] ]
		self.prepareForGeneration( 0 )

	def iterate( self ):
		''''''

		if ( len( self.populations[ 0 ] ) == 0 ):
			self.generatePopulation()


	def prepareForGeneration( self, n ):
		'''This method is called when a generation is setup.  The default implementation does nothing, but the method may be overriden in order to carry out any preparations necessary for a new  generation.'''


	def report( self, generation, bestIndividual, fitness ):
		'''Prints out a report of the progress at each generation, with the most  fit object passed in as bestIndividual.  The default implementation of  this method simply prints out the generation number, best fitness and  calls METHOD(examine) on the best individual, but you may override the  method to print out any other desired information.'''

		bf = bestIndividual.getFitness()
		print '''### Generation ; average fitness: ; best fitness: ''' % (  generation, fitness, bf )
		self.examine( bestIndividual )

	def reproduce( self ):
		''''''

		if ( len( self.populations[ ( not self.currentPopulation ) ] ) != self.populationSize ):
			del self.populations[ ( not self.currentPopulation ) ]
			self.populations[ ( not self.currentPopulation ) ] = breve.individualClass()


		best = 0
		
		totalFitness = ( totalFitness / len( self.populations[ self.currentPopulation ] ) )
		self.generation = ( self.generation + 1 )
		self.report( self.generation, self.populations[ self.currentPopulation ][ best ], totalFitness )
		
		if ( len( self.migrationHosts ) != 0 ):
			self.transferOut.clear()
			
			recipientHost = self.migrationHosts[  ]
			self.transferOut.sendOverNetwork( recipientHost,  )


		self.currentPopulation = ( not self.currentPopulation )
		self.currentIndividual = 0
		if ( self.generationLimit and ( self.generation >= self.generationLimit ) ):
			self.controller.endSimulation()

		self.prepareForGeneration( self.generation )

	def setCrossoverPercent( self, percent ):
		'''Sets the probability of crossover to percent%.  This specifies the  approximate percentage of agents to be generated through crossover during reproduction.  Crossover is not enabled by default, so the  default value is 0%.'''

		self.crossoverPercent = percent

	def setFitnessTestDuration( self, duration ):
		'''Sets the duration of a single fitness test.  Specifies that a single individual should be allowed to run for duration simulated seconds  before assessing its fitness and switching to the next individual. The default value is 20.'''

		self.testDuration = duration

	def setGenerationLimit( self, newLimit ):
		'''Sets the generation limit to newLimit.  The simulation will end after newLimit generations have passed.  If newLimit is 0, then the  generation limit is disabled.'''

		self.generationLimit = newLimit

	def setIndividualClass( self, className ):
		'''Call this method to specify the class representing an individual in your genetic algorithm.  className must be a string representing a subclass of OBJECT(GeneticAlgorithmIndividual).'''

		self.individualClass = className

	def setMigrationHosts( self, hosts ):
		''''''

		self.migrationHosts = 

	def setMigrationPercent( self, percent ):
		'''Sets the probability of migration to percent%.  This specifies the  approximate percentage that individuals will migrate to another host during reproduction, if migration is enabled.'''

		if ( percent == 0 ):
			del self.migrationServer
		else:
			if ( not self.migrationServer ):
				self.migrationServer = breve.NetworkServer()
				self.migrationServer.listen( self.migrationPort )
				self.migrationServer.setUploadRecipient( self )



		self.transferOut = breve.GAIndividualTransporter()
		self.migrationPercent = percent

	def setMigrationPort( self, port ):
		''''''

		self.migrationPort = port

	def setMutationPercent( self, percent ):
		'''Sets the probability of mutation to percent%.  This specifies the  approximate percentage of agents to be generated through mutation. The default value is 75%.'''

		self.mutationPercent = percent

	def setPopulationSize( self, size ):
		'''Sets the number of individuals in the population to size.'''

		self.populationSize = size

	def setRandomizePercent( self, percent ):
		'''Sets the probability of creating a new random individual to percent%.   The default value is 5%.'''

		self.randomizePercent = percent

	def setSeedIndividual( self, theSeed ):
		'''The seed individual is used to start the genetic algorithm at a specfic location in the search space by using the seed individual's paramters as a starting point.'''

		self.seed = theSeed

	def setSpatialRadius( self, n ):
		'''Undocumented: it will be our little secret!'''

		self.spatialRadius = n

	def setTournamentSize( self, tsize ):
		'''Sets the tournament size to tsize.  The tournament size specifies how many individuals are considered in a single selection "tournament". The larger the tournament size, the more competative selection becomes. The default value is 5.  '''

		self.tournamentSize = tsize

	def spatialTournamentSelect( self, index ):
		''''''

		best = ( index + (  - self.spatialRadius ) )
		if ( best >= self.populationSize ):
			best = ( best - self.populationSize )

		if ( best < 0 ):
			best = ( best + self.populationSize )

		
		return self.populations[ self.currentPopulation ][ best ]


	def startFitnessTest( self, individual ):
		'''Implement this method to perform any setup necessary when the  fitness test begins.'''


	def switchIndividual( self ):
		'''Switches to the next indivisual'''

		if ( self.currentIndividual != -1 ):
			self.endFitnessTest( self.populations[ self.currentPopulation ][ self.currentIndividual ] )
			if ( self.populations[ self.currentPopulation ][ self.currentIndividual ].getFitness() > self.bestFitness ):
				self.bestFitness = self.populations[ self.currentPopulation ][ self.currentIndividual ].getFitness()




		self.currentIndividual = ( self.currentIndividual + 1 )
		if ( self.currentIndividual >= len( self.populations[ self.currentPopulation ] ) ):
			self.reproduce()


		self.startFitnessTest( self.populations[ self.currentPopulation ][ self.currentIndividual ] )
		self.schedule( '''switchIndividual''', ( self.controller.getTime() + self.testDuration ) )

	def tournamentSelect( self, i ):
		''''''

		if ( self.spatialRadius == 0 ):
			return self.tournamentSelect()

		else:
			return self.spatialTournamentSelect( i )



	def tournamentSelect( self ):
		'''This method performs a tournament selection for reproduction.  It  is used internally and typically not called by any other object.  '''

		best = 
		
		return self.populations[ self.currentPopulation ][ best ]



breve.GeneticAlgorithm = GeneticAlgorithm

class GeneticAlgorithmIndividual( breve.Object ):
	'''This object represents a single individual in a OBJECT(GeneticAlgorithm). To use the class OBJECT(GeneticAlgorithm), you must also implement a  custom subclass of this class.  Your custom subclass should contain the variables which will represent the genes for the genetic algorithm, and must also implement a few methods used for genetic operators. <P> At a very minimum, you must implement your own versions of the methods METHOD(compute-fitness), METHOD(copy) and METHOD(mutate).  If you intend  to use the crossover  genetic operator, you must also implement the method METHOD(crossover). If these methods are not implemented by your subclass, the object will trigger a runtime error.'''

	def __init__( self ):
		breve.Object.__init__( self )
		self.fitness = 0
		GeneticAlgorithmIndividual.init( self )

	def copy( self, otherIndividual ):
		'''Implement this method to copy the genes from otherIndividual.  This method must be implemented by your subclass.'''

		raise Exception( '''The method "copy" must be implemented in your GeneticAlgorithmIndividual subclass!''' )


	def crossover( self, parent1, parent2 ):
		'''Implement this method to make this object become a crossover of parent1  and parent2.  This method must be implemented by your subclass if  crossover is enabled.'''

		raise Exception( '''The method "crossover" must be implemented in your GeneticAlgorithmIndividual subclass!''' )


	def getFitness( self ):
		'''Returns the fitness value associated with this individual, which has been previously assigned using METHOD(set-fitness).'''

		return self.fitness


	def init( self ):
		''''''


	def mutate( self ):
		'''Implement this method to mutate the genes of this object.  This method must be implemented by your subclass.'''

		raise Exception( '''The method "mutate" must be implemented in your GeneticAlgorithmIndividual subclass!''' )


	def randomize( self ):
		'''Implement this method to randomize the genes of this object.  This method must be implemented by your subclass.'''

		raise Exception( '''The method "randomize" must be implemented in your GeneticAlgorithmIndividual subclass!''' )


	def setFitness( self, fitnessValue ):
		'''Assigns a fitness value to this individual.  This method should be called by the user at the end of the fitness test.'''

		self.fitness = fitnessValue


breve.GeneticAlgorithmIndividual = GeneticAlgorithmIndividual

class GAIndividualTransporter( breve.Object ):
	''''''

	def __init__( self ):
		breve.Object.__init__( self )
		self.individuals = []

	def add( self, i ):
		''''''

		self.individuals.append( i )

	def clear( self ):
		''''''

		self.individuals = []

	def getIndividual( self, n ):
		''''''

		if ( n < len( self.individuals ) ):
			return self.individuals[ n ]




breve.GAIndividualTransporter = GAIndividualTransporter


