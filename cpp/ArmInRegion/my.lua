-- Primitive shapes
-- These are functions R^3 -> R.
-- Here, the convention that -ve value is inside the shape has been followed

	-- A sphere centred at the origin
	function sphere( r )
		return function( x, y, z )
			return math.sqrt( x*x + y*y + z*z ) - r;
		end
	end

	-- A cuboid centred at the origin
	function cuboid( w, h, d )
		local hw = w/2
		local hh = h/2
		local hd = d/2
		
		return function( x, y, z )
			
			return math.max( math.abs(x) - hw, math.abs(y) - hh, math.abs(z) - hd )
		end
	end

	-- A cylinder centred at the origin
	function cylinder( r, h )
		local fcub = cuboid( 2*r, 2*r, h )
		
		return function( x, y, z )
			
			-- Intersection
			return math.max( fcub( x, y, z ), math.sqrt( x*x + y*y ) - r )
		end
	end

	-- A plane whose equation is [ nx ny nz nw ] . [ x y z 1 ] = 0
	function plane( nx, ny, nz, nw )
		return function( x, y, z )
			return x*nx + y*ny + z*nz + nw;
		end
	end

-- Transform the shapes

	-- Translate the function
	function translate( f, tx, ty, tz )
		return function( x, y, z )
			return f( x-tx, y-ty, z-tz )
		end
	end
	
	-- I suppose a rotation and scaling function might be useful

-- Boolean functions to join shapes together

	-- Finds points in both shapea or shapeb
	function union( shapea, shapeb )

		return function( x, y, z )
			return math.min( shapea(x,y,z), shapeb(x,y,z) )
		end
		
	end

	-- Finds points in both shapea and shapeb
	function intersection( shapea, shapeb )

		return function( x, y, z )
			return math.max( shapea(x,y,z), shapeb(x,y,z) )
		end
		
	end

	-- Finds points which aren't in shapea
	function complement( shapea )

		return function( x, y, z )
			return -shapea( x, y, z )
		end
		
	end

	-- Finds points in shapea which aren't in shapeb
	function subtraction( shapea, shapeb )

		return function( x, y, z )
			return math.max( shapea(x,y,z), -shapeb(x,y,z) )
		end
		
	end


-- This is the region definition
-- See above to find out how it works

cyl1 = translate(
	cylinder( 1, 0.3 ),
	-1, 0, 0
)

cyl2 = translate(
	cylinder( 1, 10 ),
	 1, 0, 0
)

cub1 = cuboid( 2, 2, 2 )

-- This is the region
region = complement( union( subtraction( union( cyl1, cub1 ), cyl2 ), plane( 0, 0, 1, 0 ) ) )

-- These are the defined bounds of the region
bounds = { xmin = -2, xmax = 2, ymin = -2, ymax = 2, zmin = -0.1, zmax = 2 }
sampling = { nx = 64, ny = 64, nz = 10 }