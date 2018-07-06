with_rapidjson {
    !build_pass: message(RapidJson module is ready for use)

    #RAPIDJSON_PATH = $$(RAPIDJSON_PATH)
    DEFINES += WITH_RAPIDJSON
}
