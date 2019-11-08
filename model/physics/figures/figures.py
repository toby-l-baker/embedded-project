#################################################
#												#
#	Designs all figures in the physics paper	#
#												#
#################################################


from pysketcher import *

def fromBehind():
	H = 15
	W = 7
	drawing_tool.set_coordinate_system(xmin=0, xmax=W, ymin=0, ymax=H,axis=True)
	drawing_tool.set_grid(True)
	drawing_tool.set_linecolor('blue')

	L = 5*H/7          # length
	P = (W/6, 0.85*H)  # rotation point
	a = 30             # angle

	drawing_tool.savefig('bike_from_behind')

def set_dashed_thin_blackline(*objects):
    """Set linestyle of objects to dashed, black, width=1."""
    for obj in objects:
        obj.set_linestyle('dashed')
        obj.set_linecolor('black')
        obj.set_linewidth(1)

if __name__=="__main__":
	fromBehind()