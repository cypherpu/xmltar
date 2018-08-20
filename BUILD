# bazel build :all
# (see Target Pattern Syntax in the Command-Line Reference
# bazel query --noimplicit_deps --nohost_deps 'deps(//:HTTPFormServer)' --output graph > graph.in
# dot -Tpng < graph.in > graph.png
# nautilus graph.png

cc_binary(
	name = "Xmltar.Test",
	srcs = ["src/Xmltar.Test.cpp"]
			+glob(["src/common/**/*.cpp"] )
			+glob(["src/common/**/*.hpp"])
			,
	includes = [],
	copts = ["-g","-std=c++1z","-Isrc","-Isrc/common"],
	linkopts = [
				"-lgtest",
				]
)

#cc_binary(
#	name = "StateMachine",
#	srcs = ["src/xmltar.cpp"]
#			+glob(["src/common/StateMachine/*.cpp"] )
#			+glob(["src/common/StateMachine/*.hpp"])
#			,
#	includes = [],
#	copts = ["-g","-std=c++1z","-Isrc","-Isrc/common"],
#)

