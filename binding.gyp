{
  "targets": [
	{
		"target_name":"node-addon-tracer",
		"type" : "static_library",
		"include_dirs":[
			"./",
			"<!(node -e \"require('nan')\")"
		],
		"direct_dependent_settings":{
			"include_dirs":[
				"./",
				"<!(node -e \"require('nan')\")"
			],
		},
		"sources":[			
			  "tracer.h"
			, "tracer.cpp"
			
			, "loglevels.h"
			
			, "threadsafe_queue.h"
			
			, "uvasync.cpp"
			, "uvasync.h"
			
			, "tracer.ts"
			
			, "README.md"
			, "package.json"
		],
	},
    {
      "target_name": "tracer-tester",
	  "dependencies":[
		"node-addon-tracer"
	  ],
      "sources": [ 
		  "tests/tester.cc"
		, "tests/tester.ts"
	  ]
    }
  ]
}