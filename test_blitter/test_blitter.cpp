#define LLC_CONSOLE_LOG_ENABLED
#define LLC_ERROR_PRINTF_ENABLED
#define LLC_WARNING_PRINTF_ENABLED
#define LLC_INFO_PRINTF_ENABLED

#include "blitter.h"

#include "llc_timer.h"
#include "llc_process.h"

int												testQuery						(::blt::SBlitter & app, ::llc::string & output, ::llc::vcst_t command, ::llc::vcst_t database, s3_t detail, ::llc::vcst_t path, ::llc::rangeu3_t range, ::llc::vcst_t expand)		{
	output.clear();
	app.Query.Command								= command;
	app.Query.Database								= database;
	app.Query.Detail								= detail;
	app.ExpansionKeyStorage.clear();
	if_fail_fef(::llc::split(::llc::vcst_t{expand}, '.', app.ExpansionKeyStorage), "%s", "Out of memory?");
	app.Query.ExpansionKeys							= app.ExpansionKeyStorage;
	app.Query.Path									= path;
	app.Query.Range									= range;
	const s3_t										result							= ::blt::queryProcess(app.LoadCache, app.Databases, app.ExpressionReader, app.Query, app.Folder, output);
	if_true_fef(0 > result, "%s", "Execution of query failed.");
	if(output.size()) {
		output.push_back(0);
		if(output.size() < 1024*3)
			info_printf("command: %.*s, Database: %.*s, Detail: %lli, Path: %.*s, Range: %u, %u, Output: %s"
				, LLC_VIEW_ARGS(app.Query.Command			)
				, LLC_VIEW_ARGS(app.Query.Database		)
				, app.Query.Detail
				, LLC_VIEW_ARGS(app.Query.Path			)
				, app.Query.Range.Offset
				, app.Query.Range.Count
				, output.begin()
				);
		OutputDebugStringA(output.begin());
		OutputDebugStringA("\n");
	}
	return 0;
}

int												testPush						(::blt::SBlitter & app, ::llc::string & output, ::llc::vcst_t command, ::llc::vcst_t database, ::llc::vcst_t record)		{
	output.clear();
	app.Query.Command								= command;
	app.Query.Database								= database;
	app.Query.Record								= record;
	app.Query.RecordReader.Reset();
	::llc::jsonParse(app.Query.RecordReader, record);
	const s3_t										result							= ::blt::queryProcess(app.LoadCache, app.Databases, app.ExpressionReader, app.Query, app.Folder, output);
	if_true_fef(0 > result, "%s", "Execution of query failed.");
	if(output.size()) {
		output.push_back(0);
		if(output.size() < 1024*3)
			info_printf("%.*s", GPK_VIEW_ARGS(output));
		OutputDebugStringA(output.begin());
		OutputDebugStringA("\n");
	}
	return 0;
}

int												main							()		{
	::blt::SBlitter										app								= {};
	::llc::vcst_t											configFile						= "./blitter.json";
	if_fail_fef(::blt::loadConfig(app, configFile), "%s", "Failed to load blitter configuration.");
	::llc::string							output							= {};
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, -1	, "", {10	, 20}, ::llc::vcst_t{"referral.referral"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 0	, "", {10	, 20}, ::llc::vcst_t{"website"					});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 10	, "", {10	, 20}, ::llc::vcst_t{"referral.referral"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, -1	, "", {1	, 20}, ::llc::vcst_t{"publisher"				});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 0	, "", {10	, 20}, ::llc::vcst_t{"publisher.company"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 3	, "", {10	, 20}, ::llc::vcst_t{"publisher.company.owner"	});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, -1	, "", {1	, 20}, ::llc::vcst_t{""						});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 0	, "", {10	, 20}, ::llc::vcst_t{"company"					});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 3	, "", {10	, 20}, ::llc::vcst_t{"company.owner"			});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, -1	, "", {1	, 20}, ::llc::vcst_t{""						});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 0	, "", {10	, 20}, ::llc::vcst_t{""						});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 3	, "", {10	, 20}, ::llc::vcst_t{"owner"					});
	//
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, -1	, "", {0	, 10000}, ::llc::vcst_t{"referral.referral"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 0	, "", {1	, 10000}, ::llc::vcst_t{"website"					});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 10	, "", {0	, 10000}, ::llc::vcst_t{"referral.referral"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, -1	, "", {1	, 10000}, ::llc::vcst_t{"publisher"				});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 0	, "", {0	, 10000}, ::llc::vcst_t{"publisher.company"		});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 3	, "", {1	, 10000}, ::llc::vcst_t{"publisher.company.owner"	});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, -1	, "", {0	, 10000}, ::llc::vcst_t{""							});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 0	, "", {1	, 10000}, ::llc::vcst_t{"company"					});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 3	, "", {0	, 10000}, ::llc::vcst_t{"company.owner"			});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, -1	, "", {1	, 10000}, ::llc::vcst_t{""							});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 0	, "", {0	, 10000}, ::llc::vcst_t{""							});
	//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 3	, "", {1	, 10000}, ::llc::vcst_t{"owner"					});
	::llc::STimer										timerTotal;
	::llc::STimer										timerStep;
	const u2_t 										iterations						= 1000000;
	::llc::array_pod<double>							times;
	{
		timerTotal.Reset();
		double												timeTotal						= 0;
		for(u2_t  i = 0; i < iterations; ++i) {
			timerStep.Reset();
			//::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"company"}, ::llc::vcst_t{"1"});
			//::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"company"}, ::llc::vcst_t{"{ \"name\" : \"testA\" }"});
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"company"}, "{ \"name\" : \"testB\" }");
			timerStep.Frame();
			timeTotal											+= timerStep.LastTimeSeconds;
		}
		timerTotal.Frame();
		always_printf("------ COMPANY - Insert time: %g seconds. Record count: %u. Average: %g seconds.", timeTotal, iterations, timerTotal.LastTimeSeconds / iterations);
	}

	{
		timerTotal.Reset();
		double												timeTotal						= 0;
		for(u2_t  i = 0; i < iterations; ++i) {
			timerStep.Reset();
			//::testPush(app, output, "push_back", "user"			, "1.5");
		//	//////::testQuery(app, output, "get", "user"		, -1	, "", {0, 1000}, ""								);
			//::testPush(app, output, "push_back", "user"			, "{ \"name\" : \"test1\" }");
		//	////::testQuery(app, output, "get", "user"		, -1	, "", {0, 1000}, ""								);
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"user"}, ::llc::vcst_t{"{ \"name\" : \"test2\" }"});
			timerStep.Frame();
			timeTotal											+= timerStep.LastTimeSeconds;
		}
		timerTotal.Frame();
		always_printf("------ USER - Insert time: %g seconds. Record count: %u. Average: %g seconds.", timeTotal, iterations, timerTotal.LastTimeSeconds / iterations);
	}

	{
		timerTotal.Reset();
		double												timeTotal						= 0;
		for(u2_t  i = 0; i < iterations; ++i) {
			timerStep.Reset();
			//::testPush(app, output, "push_back", "publisher"	, "false");
			//::testPush(app, output, "push_back", "publisher"			, "{ \"name\" : \"test2\" }");
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"publisher"}, ::llc::vcst_t{"{ \"name\" : \"test7\" }"});
			//::testPush(app, output, "push_back", "publisher"	, "true");
			timerStep.Frame();
			timeTotal											+= timerStep.LastTimeSeconds;
		}
		timerTotal.Frame();
		always_printf("------ PUBLISHER - Insert time: %g seconds. Record count: %u. Average: %g seconds.", timeTotal, iterations, timerTotal.LastTimeSeconds / iterations);
	}

	{
		timerTotal.Reset();
		double												timeTotal						= 0;
		for(u2_t  i = 0; i < iterations; ++i) {
			timerStep.Reset();
			//::testPush(app, output, "push_back", "user"			, "1.5");
			//	//////::testQuery(app, output, "get", "user"		, -1	, "", {0, 1000}, ""								);
			//::testPush(app, output, "push_back", "user"			, "{ \"name\" : \"test1\" }");
			//	////::testQuery(app, output, "get", "user"		, -1	, "", {0, 1000}, ""								);
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"user"}		, ::llc::vcst_t{"{ \"name\" : \"test2\" }"});
			//	////::testQuery(app, output, "get", "user"		, -1	, "", {0, 1000}, ""								);
			////::testPush(app, output, "push_back", "website"		, "null");
			////::testPush(app, output, "push_back", "website"		, "{ \"name\" : \"test4\" }");
			////::testPush(app, output, "push_back", "website"		, "{ \"name\" : \"test5\" }");
			//::testPush(app, output, "push_back", "publisher"	, "false");
			//::testPush(app, output, "push_back", "publisher"			, "{ \"name\" : \"test2\" }");
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"publisher"}, ::llc::vcst_t{"{ \"name\" : \"test7\" }"});
			//::testPush(app, output, "push_back", "publisher"	, "true");
			//::testPush(app, output, "push_back", "company"		, "1");
			//::testPush(app, output, "push_back", "company"		, "{ \"name\" : \"testA\" }");
			::testPush(app, output, ::llc::vcst_t{"push_back"}, ::llc::vcst_t{"company"}	, ::llc::vcst_t{"{ \"name\" : \"testB\" }"});
			timerStep.Frame();
			timeTotal											+= timerStep.LastTimeSeconds;
		}
		timerTotal.Frame();
		always_printf("------ Insert time: %g seconds. Record count: %u. Average: %g seconds.", timeTotal, iterations * 3, timerTotal.LastTimeSeconds / (iterations * 3));
	}

	//info_printf("Test ------------------------------------------");
 	//{
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, -1	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{""							});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 0	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"website"					});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"user"		}, 10	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"referral.referral"		});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, -1	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"publisher"				});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 0	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"publisher.company"		});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"website"	}, 3	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"publisher.company.owner"	});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, -1	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{""							});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 0	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"company"					});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"publisher"}, 3	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"company.owner"			});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, -1	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{""							});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 0	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{""							});
		//::testQuery(app, output, ::llc::vcst_t{"get"}, ::llc::vcst_t{"company"	}, 3	, {}, {0, ::blt::MAX_TABLE_RECORD_COUNT}, ::llc::vcst_t{"owner"					});
	//}
	//::blt::blitterFlush(app);
	return 0;
}


