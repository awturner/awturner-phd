alignment = {
	source = 'Part-1-1.vtk',
	target = 'Part-1-2.vtk',
	
	rigid = {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 },
	},
	
	gridspacing = { 80, 40, 20, 10 },
	
	gridspacingnow = 10,
	
	nonrigid = {
		{ 
			used = false, 
			position = { -80, -100, -80 },
			value    = { -3.73973, -1.55813, 2.1851 }
		}, 
		{ 
			used = false, 
			position = { -80, -100, -80 },
			value    = { -3.73973, -1.55813, 2.1851 }
		}
	},
};

-- Creates a generic state machine
function CreateStateMachine( )

	statemachine = {
		-- Delegates the step function to the state
		step = function( self )
			return self.state.step( self )
		end,
		
		-- Gets the description of the current state
		description = function( self )
			return self.state.description
		end,
		
		states = {
			initial = {
				description = 'The state machine is in its initial state!';
				step = function( sm ) sm.state = sm.states.stopped end
			},
			
			stopped = {
				description = 'The state machine has stopped!';
				step = function( sm ) end
			},
			
			err = {
				description = 'The state machine has stopped with an error!';
				step = function( sm ) end
			},
		},
		
		isStopped = function( self )
			return self.state == self.states.stopped or self.state == self.states.err;
		end,
	};
	
	statemachine.state = statemachine.states.initial;
	
	return statemachine;
end

StateMachine = CreateStateMachine( );

StateMachine.numberofcalls = 0;

StateMachine.states.initial = {
	description = 'This is the first state',
	
	step = function( sm )
		sm.state = sm.states.setup;
	end
};

StateMachine.states.setup = {
	description = 'Setting things up',
	
	step = function( sm )
		if ( sm.sourceMeshFilename == nil ) then
			sm.state = sm.states.err;
			--return false;
		end
		
		if ( sm.targetMeshFilename == nil ) then
			sm.state = sm.states.err;
			--return false;
		end
		
		if ( ( sm.meshspacings == nil or #sm.meshspacings == 0 ) and ( sm.controlpoints == nil or #sm.controlpoints == 0 ) ) then
			sm.state = sm.states.err;
			--return false;
		end
		
		-- All good so far, let's initialize some values
		sm.ptrMeshSpacing  = 0;
		sm.totalIterations = 0;
		sm.lastRmsError    = 0;
		sm.allowScale      = false;
		
		sm.state = sm.states.preRigidAlignment;
		return true;
	end
};

StateMachine.states.preRigidAlignment = {
	description = "Getting ready for rigid alignment",
	
	step = function( sm )
		
		sm.rigidIterations = 0;
		sm.state = sm.states.rigidAlignment;
		
	end
}

StateMachine.states.rigidAlignment = {
	description = "Performing rigid alignment",
	
	step = function( sm )
		sm.state = sm.states.err;
	end
}

-- Make sure to set it again!
StateMachine.state = StateMachine.states.initial;

loadstatemachine = true;