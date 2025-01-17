/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License
 * 2.0 and the following additional limitation. Functionality enabled by the
 * files subject to the Elastic License 2.0 may only be used in production when
 * invoked by an Elasticsearch process with a license key installed that permits
 * use of machine learning features. You may not use this file except in
 * compliance with the Elastic License 2.0 and the foregoing additional
 * limitation.
 */

#include <core/CContainerPrinter.h>
#include <core/CJsonOutputStreamWrapper.h>
#include <core/COsFileFuncs.h>
#include <core/CScopedRapidJsonPoolAllocator.h>
#include <core/CSmallVector.h>
#include <core/CTimeUtils.h>

#include <model/CAnomalyDetector.h>
#include <model/CAnomalyDetectorModelConfig.h>
#include <model/CHierarchicalResultsNormalizer.h>
#include <model/CStringStore.h>
#include <model/ModelTypes.h>

#include <api/CGlobalCategoryId.h>
#include <api/CJsonOutputWriter.h>

#include <test/BoostTestCloseAbsolute.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(CJsonOutputWriterTest)

namespace {
using TDouble1Vec = ml::core::CSmallVector<double, 1>;
using TStr1Vec = ml::core::CSmallVector<std::string, 1>;
const TStr1Vec EMPTY_STRING_LIST;

void testBucketWriteHelper(bool isInterim) {
    // groups output by bucket/detector

    std::ostringstream sstream;

    // The output writer won't close the JSON structures until is is destroyed
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        std::string partitionFieldName("tfn");
        std::string partitionFieldValue("");
        std::string overFieldName("pfn");
        std::string overFieldValue("pfv");
        std::string byFieldName("airline");
        std::string byFieldValue("GAL");
        std::string correlatedByFieldValue("BAW");
        std::string fieldName("responsetime");
        std::string function("mean");
        std::string functionDescription("mean(responsetime)");
        std::string emptyString;
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;

        {
            ml::api::CHierarchicalResultsWriter::SResults result11(
                false, false, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 1, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.5, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result112(
                false, true, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 1, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.5, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result12(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 1, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.8, 0.0,
                -5.0, fieldName, influences, false, true, 2, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result13(
                ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
                partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 1, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.5, 0.0, -5.0,
                fieldName, influences, false, false, 3, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result14(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 1, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0,
                fieldName, influences, false, false, 4, 100, EMPTY_STRING_LIST);

            // 1st bucket
            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result112));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));
            writer.acceptBucketTimeInfluencer(1, 0.01, 13.44, 70.0);
        }

        {
            ml::api::CHierarchicalResultsWriter::SResults result21(
                false, false, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 2, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.6, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result212(
                false, true, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 2, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.6, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result22(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 2, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.8, 0.0,
                -5.0, fieldName, influences, false, true, 2, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result23(
                ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
                partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 2, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0,
                fieldName, influences, false, false, 3, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result24(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 2, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0,
                fieldName, influences, false, false, 4, 100, EMPTY_STRING_LIST);

            // 2nd bucket
            BOOST_TEST_REQUIRE(writer.acceptResult(result21));
            BOOST_TEST_REQUIRE(writer.acceptResult(result21));
            BOOST_TEST_REQUIRE(writer.acceptResult(result212));
            BOOST_TEST_REQUIRE(writer.acceptResult(result22));
            BOOST_TEST_REQUIRE(writer.acceptResult(result22));
            BOOST_TEST_REQUIRE(writer.acceptResult(result23));
            BOOST_TEST_REQUIRE(writer.acceptResult(result23));
            BOOST_TEST_REQUIRE(writer.acceptResult(result24));
            BOOST_TEST_REQUIRE(writer.acceptResult(result24));
            writer.acceptBucketTimeInfluencer(2, 0.01, 13.44, 70.0);
        }

        {
            ml::api::CHierarchicalResultsWriter::SResults result31(
                false, false, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 3, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.8, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result312(
                false, true, partitionFieldName, partitionFieldValue,
                overFieldName, overFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 3, function, functionDescription,
                TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0), 2.24, 0.8, 0.0,
                79, fieldName, influences, false, false, 1, 100);

            ml::api::CHierarchicalResultsWriter::SResults result32(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 3, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0,
                -5.0, fieldName, influences, false, true, 2, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result33(
                ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
                partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 3, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0,
                fieldName, influences, false, false, 3, 100, EMPTY_STRING_LIST);

            ml::api::CHierarchicalResultsWriter::SResults result34(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue,
                correlatedByFieldValue, 3, function, functionDescription, 42.0, 79,
                TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0,
                fieldName, influences, false, false, 4, 100, EMPTY_STRING_LIST);

            // 3rd bucket
            BOOST_TEST_REQUIRE(writer.acceptResult(result31));
            BOOST_TEST_REQUIRE(writer.acceptResult(result31));
            BOOST_TEST_REQUIRE(writer.acceptResult(result312));
            BOOST_TEST_REQUIRE(writer.acceptResult(result32));
            BOOST_TEST_REQUIRE(writer.acceptResult(result32));
            BOOST_TEST_REQUIRE(writer.acceptResult(result33));
            BOOST_TEST_REQUIRE(writer.acceptResult(result33));
            BOOST_TEST_REQUIRE(writer.acceptResult(result34));
            BOOST_TEST_REQUIRE(writer.acceptResult(result34));
            writer.acceptBucketTimeInfluencer(3, 0.01, 13.44, 70.0);
        }

        // Finished adding results
        BOOST_TEST_REQUIRE(writer.endOutputBatch(isInterim, 10U));
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str());
    BOOST_TEST_REQUIRE(!arrayDoc.HasParseError());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    arrayDoc.Accept(writer);
    LOG_DEBUG(<< "Results:\n" << strbuf.GetString());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    // There are 3 buckets and 3 record arrays in the order: r1, b1, r2, b2, r3, b3
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(6), arrayDoc.Size());

    int bucketTimes[] = {1000, 1000, 2000, 2000, 3000, 3000};

    // Assert buckets
    for (rapidjson::SizeType i = 1; i < arrayDoc.Size(); i = i + 2) {
        int buckettime = bucketTimes[i];
        const rapidjson::Value& bucketWrapper = arrayDoc[i];
        BOOST_TEST_REQUIRE(bucketWrapper.HasMember("bucket"));

        const rapidjson::Value& bucket = bucketWrapper["bucket"];
        BOOST_TEST_REQUIRE(bucket.IsObject());
        BOOST_TEST_REQUIRE(bucket.HasMember("job_id"));
        BOOST_REQUIRE_EQUAL("job", bucket["job_id"].GetString());

        // 3 detectors each have 2 records (simple count detector isn't added)
        // except the population detector which has a single record and clauses
        BOOST_REQUIRE_EQUAL(buckettime, bucket["timestamp"].GetInt());
        BOOST_TEST_REQUIRE(bucket.HasMember("bucket_influencers"));
        const rapidjson::Value& bucketInfluencers = bucket["bucket_influencers"];
        BOOST_TEST_REQUIRE(bucketInfluencers.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), bucketInfluencers.Size());
        const rapidjson::Value& bucketInfluencer =
            bucketInfluencers[rapidjson::SizeType(0)];
        BOOST_REQUIRE_CLOSE_ABSOLUTE(
            13.44, bucketInfluencer["raw_anomaly_score"].GetDouble(), 0.00001);
        BOOST_REQUIRE_CLOSE_ABSOLUTE(0.01, bucketInfluencer["probability"].GetDouble(), 0.00001);
        BOOST_REQUIRE_CLOSE_ABSOLUTE(
            70.0, bucketInfluencer["initial_anomaly_score"].GetDouble(), 0.00001);
        BOOST_TEST_REQUIRE(bucketInfluencer.HasMember("anomaly_score"));
        BOOST_REQUIRE_CLOSE_ABSOLUTE(
            70.0, bucketInfluencer["anomaly_score"].GetDouble(), 0.00001);
        BOOST_REQUIRE_EQUAL("bucket_time",
                            bucketInfluencer["influencer_field_name"].GetString());

        BOOST_REQUIRE_EQUAL(79, bucket["event_count"].GetInt());
        BOOST_TEST_REQUIRE(bucket.HasMember("anomaly_score"));
        BOOST_REQUIRE_CLOSE_ABSOLUTE(70.0, bucket["anomaly_score"].GetDouble(), 0.00001);
        BOOST_TEST_REQUIRE(bucket.HasMember("initial_anomaly_score"));
        BOOST_REQUIRE_CLOSE_ABSOLUTE(70.0, bucket["initial_anomaly_score"].GetDouble(), 0.00001);
        if (isInterim) {
            BOOST_TEST_REQUIRE(bucket.HasMember("is_interim"));
            BOOST_REQUIRE_EQUAL(isInterim, bucket["is_interim"].GetBool());
        } else {
            BOOST_TEST_REQUIRE(!bucket.HasMember("is_interim"));
        }

        BOOST_REQUIRE_EQUAL(std::uint64_t(10ll), bucket["processing_time_ms"].GetUint64());
    }

    for (rapidjson::SizeType i = 0; i < arrayDoc.Size(); i = i + 2) {
        int buckettime = bucketTimes[i];

        const rapidjson::Value& recordsWrapper = arrayDoc[i];
        BOOST_TEST_REQUIRE(recordsWrapper.HasMember("records"));

        const rapidjson::Value& records = recordsWrapper["records"];
        BOOST_TEST_REQUIRE(records.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(5), records.Size());

        // 1st record is for population detector
        {
            const rapidjson::Value& record = records[rapidjson::SizeType(0)];
            BOOST_TEST_REQUIRE(record.HasMember("job_id"));
            BOOST_REQUIRE_EQUAL("job", record["job_id"].GetString());
            BOOST_TEST_REQUIRE(record.HasMember("detector_index"));
            BOOST_REQUIRE_EQUAL(1, record["detector_index"].GetInt());
            BOOST_TEST_REQUIRE(record.HasMember("timestamp"));
            BOOST_REQUIRE_EQUAL(buckettime, record["timestamp"].GetInt());
            BOOST_TEST_REQUIRE(record.HasMember("probability"));
            BOOST_REQUIRE_EQUAL(0.0, record["probability"].GetDouble());
            BOOST_TEST_REQUIRE(record.HasMember("by_field_name"));
            BOOST_REQUIRE_EQUAL("airline", record["by_field_name"].GetString());
            BOOST_TEST_REQUIRE(!record.HasMember("by_field_value"));
            BOOST_TEST_REQUIRE(!record.HasMember("correlated_by_field_value"));
            BOOST_TEST_REQUIRE(record.HasMember("function"));
            BOOST_REQUIRE_EQUAL("mean", record["function"].GetString());
            BOOST_TEST_REQUIRE(record.HasMember("function_description"));
            BOOST_REQUIRE_EQUAL("mean(responsetime)",
                                record["function_description"].GetString());
            BOOST_TEST_REQUIRE(record.HasMember("over_field_name"));
            BOOST_REQUIRE_EQUAL("pfn", record["over_field_name"].GetString());
            BOOST_TEST_REQUIRE(record.HasMember("over_field_value"));
            BOOST_REQUIRE_EQUAL("pfv", record["over_field_value"].GetString());
            BOOST_TEST_REQUIRE(record.HasMember("bucket_span"));
            BOOST_REQUIRE_EQUAL(100, record["bucket_span"].GetInt());
            // It's hard to predict what these will be, so just assert their
            // presence
            BOOST_TEST_REQUIRE(record.HasMember("initial_record_score"));
            BOOST_TEST_REQUIRE(record.HasMember("record_score"));
            if (isInterim) {
                BOOST_TEST_REQUIRE(record.HasMember("is_interim"));
                BOOST_REQUIRE_EQUAL(isInterim, record["is_interim"].GetBool());
            } else {
                BOOST_TEST_REQUIRE(!record.HasMember("is_interim"));
            }

            BOOST_TEST_REQUIRE(record.HasMember("causes"));
            const rapidjson::Value& causes = record["causes"];
            BOOST_TEST_REQUIRE(causes.IsArray());
            BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), causes.Size());
            for (rapidjson::SizeType k = 0; k < causes.Size(); k++) {
                const rapidjson::Value& cause = causes[k];
                BOOST_TEST_REQUIRE(cause.HasMember("probability"));
                BOOST_REQUIRE_EQUAL(0.0, cause["probability"].GetDouble());
                BOOST_TEST_REQUIRE(cause.HasMember("field_name"));
                BOOST_REQUIRE_EQUAL("responsetime", cause["field_name"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("by_field_name"));
                BOOST_REQUIRE_EQUAL("airline", cause["by_field_name"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("by_field_value"));
                BOOST_REQUIRE_EQUAL("GAL", cause["by_field_value"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("correlated_by_field_value"));
                BOOST_REQUIRE_EQUAL("BAW", cause["correlated_by_field_value"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("partition_field_name"));
                BOOST_REQUIRE_EQUAL("tfn", cause["partition_field_name"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("partition_field_value"));
                BOOST_REQUIRE_EQUAL("", cause["partition_field_value"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("function"));
                BOOST_REQUIRE_EQUAL("mean", cause["function"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("function_description"));
                BOOST_REQUIRE_EQUAL("mean(responsetime)",
                                    cause["function_description"].GetString());
                BOOST_TEST_REQUIRE(cause.HasMember("typical"));
                BOOST_TEST_REQUIRE(cause["typical"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), cause["typical"].Size());
                BOOST_REQUIRE_EQUAL(
                    6953.0, cause["typical"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(cause.HasMember("actual"));
                BOOST_TEST_REQUIRE(cause["actual"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), cause["actual"].Size());
                BOOST_REQUIRE_EQUAL(
                    10090.0, cause["actual"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(cause.HasMember("function"));
            }
        }

        // Next 2 records are for metric detector
        {
            for (rapidjson::SizeType k = 1; k < 3; k++) {
                const rapidjson::Value& record = records[k];
                BOOST_TEST_REQUIRE(record.HasMember("job_id"));
                BOOST_REQUIRE_EQUAL("job", record["job_id"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("detector_index"));
                BOOST_REQUIRE_EQUAL(2, record["detector_index"].GetInt());
                BOOST_TEST_REQUIRE(record.HasMember("timestamp"));
                BOOST_REQUIRE_EQUAL(buckettime, record["timestamp"].GetInt());
                BOOST_TEST_REQUIRE(record.HasMember("probability"));
                BOOST_REQUIRE_EQUAL(0.0, record["probability"].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("by_field_name"));
                BOOST_REQUIRE_EQUAL("airline", record["by_field_name"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("by_field_value"));
                BOOST_REQUIRE_EQUAL("GAL", record["by_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("correlated_by_field_value"));
                BOOST_REQUIRE_EQUAL("BAW", record["correlated_by_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("typical"));
                BOOST_TEST_REQUIRE(record["typical"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), record["typical"].Size());
                BOOST_REQUIRE_EQUAL(
                    6953.0, record["typical"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("actual"));
                BOOST_TEST_REQUIRE(record["actual"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), record["actual"].Size());
                BOOST_REQUIRE_EQUAL(
                    10090.0, record["actual"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("field_name"));
                BOOST_REQUIRE_EQUAL("responsetime", record["field_name"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("function"));
                BOOST_REQUIRE_EQUAL("mean", record["function"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("function_description"));
                BOOST_REQUIRE_EQUAL("mean(responsetime)",
                                    record["function_description"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("partition_field_name"));
                BOOST_REQUIRE_EQUAL("tfn", record["partition_field_name"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("partition_field_value"));
                BOOST_REQUIRE_EQUAL("", record["partition_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("bucket_span"));
                BOOST_REQUIRE_EQUAL(100, record["bucket_span"].GetInt());
                // It's hard to predict what these will be, so just assert their
                // presence
                BOOST_TEST_REQUIRE(record.HasMember("initial_record_score"));
                BOOST_TEST_REQUIRE(record.HasMember("record_score"));
                if (isInterim) {
                    BOOST_TEST_REQUIRE(record.HasMember("is_interim"));
                    BOOST_REQUIRE_EQUAL(isInterim, record["is_interim"].GetBool());
                } else {
                    BOOST_TEST_REQUIRE(!record.HasMember("is_interim"));
                }
            }
        }

        // Last 2 records are for event rate detector
        {
            for (rapidjson::SizeType k = 3; k < 5; k++) {
                const rapidjson::Value& record = records[k];
                BOOST_TEST_REQUIRE(record.HasMember("job_id"));
                BOOST_REQUIRE_EQUAL("job", record["job_id"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("detector_index"));
                BOOST_REQUIRE_EQUAL(4, record["detector_index"].GetInt());
                BOOST_TEST_REQUIRE(record.HasMember("timestamp"));
                BOOST_REQUIRE_EQUAL(buckettime, record["timestamp"].GetInt());
                BOOST_TEST_REQUIRE(record.HasMember("probability"));
                BOOST_REQUIRE_EQUAL(0.0, record["probability"].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("by_field_name"));
                BOOST_REQUIRE_EQUAL("airline", record["by_field_name"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("by_field_value"));
                BOOST_REQUIRE_EQUAL("GAL", record["by_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("correlated_by_field_value"));
                BOOST_REQUIRE_EQUAL("BAW", record["correlated_by_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("typical"));
                BOOST_TEST_REQUIRE(record["typical"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), record["typical"].Size());
                BOOST_REQUIRE_EQUAL(
                    6953.0, record["typical"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("actual"));
                BOOST_TEST_REQUIRE(record["actual"].IsArray());
                BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), record["actual"].Size());
                BOOST_REQUIRE_EQUAL(
                    10090.0, record["actual"][rapidjson::SizeType(0)].GetDouble());
                BOOST_TEST_REQUIRE(record.HasMember("function"));
                // This would be count in the real case with properly generated input data
                BOOST_REQUIRE_EQUAL("mean", record["function"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("function_description"));
                BOOST_REQUIRE_EQUAL("mean(responsetime)",
                                    record["function_description"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("partition_field_name"));
                BOOST_REQUIRE_EQUAL("tfn", record["partition_field_name"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("partition_field_value"));
                BOOST_REQUIRE_EQUAL("", record["partition_field_value"].GetString());
                BOOST_TEST_REQUIRE(record.HasMember("bucket_span"));
                BOOST_REQUIRE_EQUAL(100, record["bucket_span"].GetInt());
                // It's hard to predict what these will be, so just assert their
                // presence
                BOOST_TEST_REQUIRE(record.HasMember("initial_record_score"));
                BOOST_TEST_REQUIRE(record.HasMember("record_score"));
                if (isInterim) {
                    BOOST_TEST_REQUIRE(record.HasMember("is_interim"));
                    BOOST_REQUIRE_EQUAL(isInterim, record["is_interim"].GetBool());
                } else {
                    BOOST_TEST_REQUIRE(!record.HasMember("is_interim"));
                }
            }
        }
    }
}

void testLimitedRecordsWriteHelper(bool isInterim) {
    // Tests CJsonOutputWriter::limitNumberRecords(size_t)
    // set the record limit for each detector to 2

    std::ostringstream sstream;

    // The output writer won't close the JSON structures until is is destroyed
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);
        writer.limitNumberRecords(2);

        std::string partitionFieldName("tfn");
        std::string partitionFieldValue("tfv");
        std::string overFieldName("pfn");
        std::string overFieldValue("pfv");
        std::string byFieldName("airline");
        std::string byFieldValue("GAL");
        std::string fieldName("responsetime");
        std::string function("mean");
        std::string functionDescription("mean(responsetime)");
        std::string emptyString;
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;

        {
            // 1st bucket
            ml::api::CHierarchicalResultsWriter::SResults result111(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 1,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.1, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result111));

            ml::api::CHierarchicalResultsWriter::SResults result112(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 1,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.2, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result112));

            ml::api::CHierarchicalResultsWriter::SResults result113(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 1,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 2.0, 0.0, 0.4, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result113));

            ml::api::CHierarchicalResultsWriter::SResults result114(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 1,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 12.0, 0.0, 0.4, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result114));
            BOOST_TEST_REQUIRE(writer.acceptResult(result114));

            overFieldName = "ofn";
            overFieldValue = "ofv";

            ml::api::CHierarchicalResultsWriter::SResults result121(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                12.0, 0.0, 0.01, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result121));

            ml::api::CHierarchicalResultsWriter::SResults result122(
                false, true, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                12.0, 0.0, 0.01, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result122));

            ml::api::CHierarchicalResultsWriter::SResults result123(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                0.5, 0.0, 0.5, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result123));

            ml::api::CHierarchicalResultsWriter::SResults result124(
                false, true, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                0.5, 0.0, 0.5, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result124));

            ml::api::CHierarchicalResultsWriter::SResults result125(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                6.0, 0.0, 0.5, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result125));

            ml::api::CHierarchicalResultsWriter::SResults result126(
                false, true, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 1, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                6.0, 0.0, 0.05, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result126));
        }

        {
            // 2nd bucket
            overFieldName.clear();
            overFieldValue.clear();

            ml::api::CHierarchicalResultsWriter::SResults result211(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 2,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 1.0, 0.0, 0.05, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result211));

            ml::api::CHierarchicalResultsWriter::SResults result212(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 2,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 7.0, 0.0, 0.001, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result212));

            ml::api::CHierarchicalResultsWriter::SResults result213(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 2,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 0.6, 0.0, 0.1, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result213));
            BOOST_TEST_REQUIRE(writer.acceptResult(result213));

            overFieldName = "ofn";
            overFieldValue = "ofv";

            ml::api::CHierarchicalResultsWriter::SResults result221(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 2, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                0.6, 0.0, 0.1, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result221));
            BOOST_TEST_REQUIRE(writer.acceptResult(result221));

            ml::api::CHierarchicalResultsWriter::SResults result222(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 2, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                0.6, 0.0, 0.1, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result222));

            ml::api::CHierarchicalResultsWriter::SResults result223(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 2, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                3.0, 0.0, 0.02, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result223));

            ml::api::CHierarchicalResultsWriter::SResults result224(
                false, true, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 2, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                20.0, 0.0, 0.02, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result224));
        }

        {
            // 3rd bucket
            overFieldName.clear();
            overFieldValue.clear();

            ml::api::CHierarchicalResultsWriter::SResults result311(
                ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
                partitionFieldValue, byFieldName, byFieldValue, emptyString, 3,
                function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
                TDouble1Vec(1, 10090.0), 30.0, 0.0, 0.02, -5.0, fieldName,
                influences, false, true, 1, 100, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result311));

            overFieldName = "ofn";
            overFieldValue = "ofv";

            ml::api::CHierarchicalResultsWriter::SResults result321(
                false, false, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, byFieldName, byFieldValue, emptyString, 3, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                31.0, 0.0, 0.0002, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result321));

            ml::api::CHierarchicalResultsWriter::SResults result322(
                false, true, partitionFieldName, partitionFieldValue, overFieldName,
                overFieldValue, emptyString, emptyString, emptyString, 3, function,
                functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
                31.0, 0.0, 0.0002, 79, fieldName, influences, false, true, 2, 100);
            BOOST_TEST_REQUIRE(writer.acceptResult(result322));
        }

        // Finished adding results
        BOOST_TEST_REQUIRE(writer.endOutputBatch(isInterim, 10U));
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    arrayDoc.Accept(writer);
    LOG_DEBUG(<< "Results:\n" << strbuf.GetString());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(6), arrayDoc.Size());

    // buckets and records are the top level objects
    // records corresponding to a bucket appear first. The bucket follows.
    // each bucket has max 2 records from either both or
    // one or the other of the 2 detectors used.
    // records are sorted by probability.
    // bucket total anomaly score is the sum of all anomalies not just those printed.
    {
        const rapidjson::Value& bucketWrapper = arrayDoc[rapidjson::SizeType(1)];
        BOOST_TEST_REQUIRE(bucketWrapper.IsObject());
        BOOST_TEST_REQUIRE(bucketWrapper.HasMember("bucket"));

        const rapidjson::Value& bucket = bucketWrapper["bucket"];
        BOOST_TEST_REQUIRE(bucket.IsObject());
        // It's hard to predict what these will be, so just assert their presence
        BOOST_TEST_REQUIRE(bucket.HasMember("anomaly_score"));
        if (isInterim) {
            BOOST_TEST_REQUIRE(bucket.HasMember("is_interim"));
            BOOST_REQUIRE_EQUAL(isInterim, bucket["is_interim"].GetBool());
        } else {
            BOOST_TEST_REQUIRE(!bucket.HasMember("is_interim"));
        }

        const rapidjson::Value& recordsWrapper = arrayDoc[rapidjson::SizeType(0)];
        BOOST_TEST_REQUIRE(recordsWrapper.IsObject());
        BOOST_TEST_REQUIRE(recordsWrapper.HasMember("records"));
        const rapidjson::Value& records = recordsWrapper["records"];
        BOOST_TEST_REQUIRE(records.IsArray());

        double EXPECTED_PROBABILITIES[] = {0.01, 0.05, 0.001, 0.02, 0.0002};

        int probIndex = 0;
        for (rapidjson::SizeType i = 0; i < records.Size(); i++) {
            BOOST_TEST_REQUIRE(records[i].HasMember("detector_index"));
            BOOST_TEST_REQUIRE(records[i].HasMember("initial_record_score"));
            BOOST_TEST_REQUIRE(records[i].HasMember("record_score"));
            BOOST_TEST_REQUIRE(records[i].HasMember("probability"));
            BOOST_REQUIRE_EQUAL(EXPECTED_PROBABILITIES[probIndex],
                                records[i]["probability"].GetDouble());
            ++probIndex;

            if (isInterim) {
                BOOST_TEST_REQUIRE(records[i].HasMember("is_interim"));
                BOOST_REQUIRE_EQUAL(isInterim, records[i]["is_interim"].GetBool());
            } else {
                BOOST_TEST_REQUIRE(!records[i].HasMember("is_interim"));
            }
        }

        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), records.Size());
    }
    {
        const rapidjson::Value& bucketWrapper = arrayDoc[rapidjson::SizeType(3)];
        BOOST_TEST_REQUIRE(bucketWrapper.IsObject());
        BOOST_TEST_REQUIRE(bucketWrapper.HasMember("bucket"));

        const rapidjson::Value& bucket = bucketWrapper["bucket"];
        BOOST_TEST_REQUIRE(bucket.IsObject());
        // It's hard to predict what these will be, so just assert their presence
        BOOST_TEST_REQUIRE(bucket.HasMember("anomaly_score"));
        if (isInterim) {
            BOOST_TEST_REQUIRE(bucket.HasMember("is_interim"));
            BOOST_REQUIRE_EQUAL(isInterim, bucket["is_interim"].GetBool());
        } else {
            BOOST_TEST_REQUIRE(!bucket.HasMember("is_interim"));
        }

        const rapidjson::Value& recordsWrapper = arrayDoc[rapidjson::SizeType(2)];
        BOOST_TEST_REQUIRE(recordsWrapper.IsObject());
        BOOST_TEST_REQUIRE(recordsWrapper.HasMember("records"));
        const rapidjson::Value& records = recordsWrapper["records"];
        BOOST_TEST_REQUIRE(records.IsArray());

        for (rapidjson::SizeType i = 0; i < records.Size(); i++) {
            //BOOST_REQUIRE_EQUAL(0.1, records1[rapidjson::SizeType(0)]["probability"].GetDouble());
            BOOST_TEST_REQUIRE(records[i].HasMember("detector_index"));
            BOOST_TEST_REQUIRE(records[i].HasMember("initial_record_score"));
            BOOST_TEST_REQUIRE(records[i].HasMember("record_score"));
            if (isInterim) {
                BOOST_TEST_REQUIRE(records[i].HasMember("is_interim"));
                BOOST_REQUIRE_EQUAL(isInterim, records[i]["is_interim"].GetBool());
            } else {
                BOOST_TEST_REQUIRE(!records[i].HasMember("is_interim"));
            }
        }

        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), records.Size());
    }
    {
        const rapidjson::Value& bucketWrapper = arrayDoc[rapidjson::SizeType(5)];
        BOOST_TEST_REQUIRE(bucketWrapper.IsObject());
        BOOST_TEST_REQUIRE(bucketWrapper.HasMember("bucket"));

        const rapidjson::Value& bucket = bucketWrapper["bucket"];
        BOOST_TEST_REQUIRE(bucket.IsObject());
        // It's hard to predict what these will be, so just assert their presence
        BOOST_TEST_REQUIRE(bucket.HasMember("anomaly_score"));
        if (isInterim) {
            BOOST_TEST_REQUIRE(bucket.HasMember("is_interim"));
            BOOST_REQUIRE_EQUAL(isInterim, bucket["is_interim"].GetBool());
        } else {
            BOOST_TEST_REQUIRE(!bucket.HasMember("is_interim"));
        }

        const rapidjson::Value& recordsWrapper = arrayDoc[rapidjson::SizeType(4)];
        BOOST_TEST_REQUIRE(recordsWrapper.IsObject());
        BOOST_TEST_REQUIRE(recordsWrapper.HasMember("records"));
        const rapidjson::Value& records = recordsWrapper["records"];
        BOOST_TEST_REQUIRE(records.IsArray());

        for (rapidjson::SizeType i = 0; i < records.Size(); i++) {
            BOOST_TEST_REQUIRE(records[i].HasMember("detector_index"));
            //BOOST_REQUIRE_EQUAL(0.1, records1[rapidjson::SizeType(0)]["probability"].GetDouble());
            BOOST_TEST_REQUIRE(records[i].HasMember("initial_record_score"));
            BOOST_TEST_REQUIRE(records[i].HasMember("record_score"));
            if (isInterim) {
                BOOST_TEST_REQUIRE(records[i].HasMember("is_interim"));
                BOOST_REQUIRE_EQUAL(isInterim, records[i]["is_interim"].GetBool());
            } else {
                BOOST_TEST_REQUIRE(!records[i].HasMember("is_interim"));
            }
        }

        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), records.Size());
    }
}

ml::model::CHierarchicalResults::TNode
createInfluencerNode(const std::string& personName,
                     const std::string& personValue,
                     double probability,
                     double normalisedAnomalyScore) {
    ml::model::CHierarchicalResults::TResultSpec spec;
    spec.s_PersonFieldName = ml::model::CStringStore::names().get(personName);
    spec.s_PersonFieldValue = ml::model::CStringStore::names().get(personValue);

    ml::model::CHierarchicalResults::TNode node;
    node.s_AnnotatedProbability.s_Probability = probability;
    node.s_NormalizedAnomalyScore = normalisedAnomalyScore;
    node.s_Spec = spec;

    return node;
}

ml::model::CHierarchicalResults::TNode
createBucketInfluencerNode(const std::string& personName,
                           double probability,
                           double normalisedAnomalyScore,
                           double rawAnomalyScore) {
    ml::model::CHierarchicalResults::TResultSpec spec;
    spec.s_PersonFieldName = ml::model::CStringStore::names().get(personName);

    ml::model::CHierarchicalResults::TNode node;
    node.s_AnnotatedProbability.s_Probability = probability;
    node.s_NormalizedAnomalyScore = normalisedAnomalyScore;
    node.s_RawAnomalyScore = rawAnomalyScore;
    node.s_Spec = spec;

    return node;
}

void testThroughputHelper(bool useScopedAllocator) {
    // Write to /dev/null (Unix) or nul (Windows)
    std::ofstream ofs(ml::core::COsFileFuncs::NULL_FILENAME);
    BOOST_TEST_REQUIRE(ofs.is_open());

    ml::core::CJsonOutputStreamWrapper outputStream(ofs);
    ml::api::CJsonOutputWriter writer("job", outputStream);

    std::string partitionFieldName("tfn");
    std::string partitionFieldValue("");
    std::string overFieldName("pfn");
    std::string overFieldValue("pfv");
    std::string byFieldName("airline");
    std::string byFieldValue("GAL");
    std::string correlatedByFieldValue("BAW");
    std::string fieldName("responsetime");
    std::string function("mean");
    std::string functionDescription("mean(responsetime)");
    std::string emptyString;
    ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;

    ml::api::CHierarchicalResultsWriter::SResults result11(
        false, false, partitionFieldName, partitionFieldValue, overFieldName,
        overFieldValue, byFieldName, byFieldValue, correlatedByFieldValue, 1, function,
        functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
        2.24, 0.5, 0.0, 79, fieldName, influences, false, false, 1, 100);

    ml::api::CHierarchicalResultsWriter::SResults result112(
        false, true, partitionFieldName, partitionFieldValue, overFieldName,
        overFieldValue, byFieldName, byFieldValue, correlatedByFieldValue, 1, function,
        functionDescription, TDouble1Vec(1, 10090.0), TDouble1Vec(1, 6953.0),
        2.24, 0.5, 0.0, 79, fieldName, influences, false, false, 1, 100);

    ml::api::CHierarchicalResultsWriter::SResults result12(
        ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
        partitionFieldValue, byFieldName, byFieldValue, correlatedByFieldValue,
        1, function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
        TDouble1Vec(1, 10090.0), 2.24, 0.8, 0.0, -5.0, fieldName, influences,
        false, true, 2, 100, EMPTY_STRING_LIST);

    ml::api::CHierarchicalResultsWriter::SResults result13(
        ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
        partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
        correlatedByFieldValue, 1, function, functionDescription, 42.0, 79,
        TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 2.24, 0.5, 0.0, -5.0,
        fieldName, influences, false, false, 3, 100, EMPTY_STRING_LIST);

    ml::api::CHierarchicalResultsWriter::SResults result14(
        ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
        partitionFieldValue, byFieldName, byFieldValue, correlatedByFieldValue,
        1, function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
        TDouble1Vec(1, 10090.0), 2.24, 0.0, 0.0, -5.0, fieldName, influences,
        false, false, 4, 100, EMPTY_STRING_LIST);

    // 1st bucket
    writer.acceptBucketTimeInfluencer(1, 0.01, 13.44, 70.0);

    // Write the record this many times
    static const size_t TEST_SIZE(75000);

    ml::core_t::TTime start(ml::core::CTimeUtils::now());
    LOG_INFO(<< "Starting throughput test at " << ml::core::CTimeUtils::toTimeString(start));

    for (size_t count = 0; count < TEST_SIZE; ++count) {
        if (useScopedAllocator) {
            using TScopedAllocator =
                ml::core::CScopedRapidJsonPoolAllocator<ml::api::CJsonOutputWriter>;
            static const std::string ALLOCATOR_ID("CAnomalyJob::writeOutResults");
            TScopedAllocator scopedAllocator(ALLOCATOR_ID, writer);

            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result112));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));

            // Finished adding results
            BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
        } else {
            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result11));
            BOOST_TEST_REQUIRE(writer.acceptResult(result112));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result12));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result13));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));
            BOOST_TEST_REQUIRE(writer.acceptResult(result14));

            // Finished adding results
            BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
        }
    }

    ml::core_t::TTime end(ml::core::CTimeUtils::now());
    LOG_INFO(<< "Finished throughput test at " << ml::core::CTimeUtils::toTimeString(end));

    LOG_INFO(<< "Writing " << TEST_SIZE << " records took " << (end - start) << " seconds");
}
}

BOOST_AUTO_TEST_CASE(testGeoResultsWrite) {

    std::string partitionFieldName("tfn");
    std::string partitionFieldValue("");
    std::string overFieldName("ofn");
    std::string overFieldValue("ofv");
    std::string byFieldName("airline");
    std::string byFieldValue("GAL");
    std::string correlatedByFieldValue("BAW");
    std::string fieldName("location");
    std::string function("lat_long");
    std::string functionDescription("lat_long(location)");
    ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;
    std::string emptyString;
    // The output writer won't close the JSON structures until is is destroyed
    {
        std::ostringstream sstream;
        {
            ml::core::CJsonOutputStreamWrapper outputStream(sstream);
            ml::api::CJsonOutputWriter writer("job", outputStream);
            TDouble1Vec actual(2, 0.0);
            actual[0] = 40.0;
            actual[1] = -40.0;
            TDouble1Vec typical(2, 0.0);
            typical[0] = 90.0;
            typical[1] = -90.0;
            ml::api::CHierarchicalResultsWriter::SResults result(
                ml::api::CHierarchicalResultsWriter::E_Result,
                partitionFieldName, partitionFieldValue, byFieldName,
                byFieldValue, correlatedByFieldValue, 1, function,
                functionDescription, 2.24, 79, typical, actual, 10.0, 10.0, 0.5,
                0.0, fieldName, influences, false, true, 1, 1, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result));
            BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
        }
        rapidjson::Document arrayDoc;
        arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());
        // Debug print record
        {
            rapidjson::StringBuffer strbuf;
            using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
            TStringBufferPrettyWriter writer(strbuf);
            arrayDoc.Accept(writer);
            LOG_DEBUG(<< "Results:\n" << strbuf.GetString());
        }
        BOOST_TEST_REQUIRE(arrayDoc.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), arrayDoc.Size());
        BOOST_TEST_REQUIRE(arrayDoc[rapidjson::SizeType(0)].HasMember("records"));
        const rapidjson::Value& record =
            arrayDoc[rapidjson::SizeType(0)]["records"][rapidjson::SizeType(0)];

        BOOST_TEST_REQUIRE(record.HasMember("typical"));
        BOOST_TEST_REQUIRE(record.HasMember("actual"));
        BOOST_TEST_REQUIRE(record.HasMember("geo_results"));
        auto geoResultsObject = record["geo_results"].GetObject();
        BOOST_TEST_REQUIRE(geoResultsObject.HasMember("actual_point"));
        BOOST_REQUIRE_EQUAL("40.000000000000,-40.000000000000",
                            geoResultsObject["actual_point"].GetString());
        BOOST_TEST_REQUIRE(geoResultsObject.HasMember("typical_point"));
        BOOST_REQUIRE_EQUAL("90.000000000000,-90.000000000000",
                            geoResultsObject["typical_point"].GetString());
    }

    {
        std::ostringstream sstream;
        {
            ml::core::CJsonOutputStreamWrapper outputStream(sstream);
            ml::api::CJsonOutputWriter writer("job", outputStream);
            TDouble1Vec actual(1, 500);
            TDouble1Vec typical(1, 64);
            ml::api::CHierarchicalResultsWriter::SResults result(
                ml::api::CHierarchicalResultsWriter::E_Result,
                partitionFieldName, partitionFieldValue, byFieldName,
                byFieldValue, correlatedByFieldValue, 1, function,
                functionDescription, 2.24, 79, typical, actual, 10.0, 10.0, 0.5,
                0.0, fieldName, influences, false, true, 1, 1, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result));
            BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
        }
        rapidjson::Document arrayDoc;
        arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());
        // Debug print record
        {
            rapidjson::StringBuffer strbuf;
            using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
            TStringBufferPrettyWriter writer(strbuf);
            arrayDoc.Accept(writer);
            LOG_DEBUG(<< "Results:\n" << strbuf.GetString());
        }
        BOOST_TEST_REQUIRE(arrayDoc.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), arrayDoc.Size());
        BOOST_TEST_REQUIRE(arrayDoc[rapidjson::SizeType(0)].HasMember("records"));
        const rapidjson::Value& record =
            arrayDoc[rapidjson::SizeType(0)]["records"][rapidjson::SizeType(0)];

        BOOST_TEST_REQUIRE(record.IsObject());
        BOOST_TEST_REQUIRE(record.HasMember("geo_results"));
        auto geoResultsObject = record["geo_results"].GetObject();
        BOOST_TEST_REQUIRE(!geoResultsObject.HasMember("actual_point"));
        BOOST_TEST_REQUIRE(!geoResultsObject.HasMember("typical_point"));
    }

    {
        std::ostringstream sstream;
        {
            ml::core::CJsonOutputStreamWrapper outputStream(sstream);
            ml::api::CJsonOutputWriter writer("job", outputStream);
            TDouble1Vec actual(1, 500);
            TDouble1Vec typical(1, 64);
            ml::api::CHierarchicalResultsWriter::SResults result(
                ml::api::CHierarchicalResultsWriter::E_Result,
                partitionFieldName, partitionFieldValue, byFieldName,
                byFieldValue, correlatedByFieldValue, 1, "mean",
                functionDescription, 2.24, 79, typical, actual, 10.0, 10.0, 0.5,
                0.0, fieldName, influences, false, true, 1, 1, EMPTY_STRING_LIST);
            BOOST_TEST_REQUIRE(writer.acceptResult(result));
            BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
        }
        rapidjson::Document arrayDoc;
        arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());
        // Debug print record
        {
            rapidjson::StringBuffer strbuf;
            using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
            TStringBufferPrettyWriter writer(strbuf);
            arrayDoc.Accept(writer);
            LOG_DEBUG(<< "Results:\n" << strbuf.GetString());
        }
        BOOST_TEST_REQUIRE(arrayDoc.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), arrayDoc.Size());
        BOOST_TEST_REQUIRE(arrayDoc[rapidjson::SizeType(0)].HasMember("records"));
        const rapidjson::Value& record =
            arrayDoc[rapidjson::SizeType(0)]["records"][rapidjson::SizeType(0)];

        BOOST_TEST_REQUIRE(record.IsObject());
        BOOST_REQUIRE_EQUAL(false, record.HasMember("geo_results"));
    }
}

BOOST_AUTO_TEST_CASE(testWriteNonAnomalousBucket) {
    std::ostringstream sstream;

    std::string function("mean");
    std::string functionDescription("mean(responsetime)");
    std::string emptyString;
    ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        ml::api::CHierarchicalResultsWriter::SResults result(
            false, false, emptyString, emptyString, emptyString, emptyString,
            emptyString, emptyString, emptyString, 1, function,
            functionDescription, TDouble1Vec(1, 42.0), TDouble1Vec(1, 42.0),
            0.0, 0.0, 1.0, 30, emptyString, influences, false, false, 1, 100);

        BOOST_TEST_REQUIRE(writer.acceptResult(result));
        writer.acceptBucketTimeInfluencer(1, 1.0, 0.0, 0.0);
        BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 10U));
        writer.finalise();
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter prettyPrinter(strbuf);
    arrayDoc.Accept(prettyPrinter);
    LOG_DEBUG(<< "Results:\n" << strbuf.GetString());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), arrayDoc.Size());

    const rapidjson::Value& bucketWrapper = arrayDoc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(bucketWrapper.IsObject());
    BOOST_TEST_REQUIRE(bucketWrapper.HasMember("bucket"));

    const rapidjson::Value& bucket = bucketWrapper["bucket"];
    BOOST_TEST_REQUIRE(bucket.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", bucket["job_id"].GetString());
    BOOST_REQUIRE_EQUAL(1000, bucket["timestamp"].GetInt());
    BOOST_TEST_REQUIRE(bucket.HasMember("bucket_influencers") == false);
    BOOST_REQUIRE_EQUAL(0, bucket["event_count"].GetInt());
    BOOST_TEST_REQUIRE(bucket.HasMember("anomaly_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.0, bucket["anomaly_score"].GetDouble(), 0.00001);
}

BOOST_AUTO_TEST_CASE(testFlush) {
    std::string testId("testflush");
    ml::core_t::TTime lastFinalizedBucketEnd(123456789);
    std::ostringstream sstream;

    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        writer.acknowledgeFlush(testId, lastFinalizedBucketEnd);
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), arrayDoc.Size());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    arrayDoc.Accept(writer);
    LOG_DEBUG(<< "Flush:\n" << strbuf.GetString());

    const rapidjson::Value& flushWrapper = arrayDoc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(flushWrapper.IsObject());
    BOOST_TEST_REQUIRE(flushWrapper.HasMember("flush"));

    const rapidjson::Value& flush = flushWrapper["flush"];
    BOOST_TEST_REQUIRE(flush.IsObject());
    BOOST_TEST_REQUIRE(flush.HasMember("id"));
    BOOST_REQUIRE_EQUAL(testId, flush["id"].GetString());
    BOOST_TEST_REQUIRE(flush.HasMember("last_finalized_bucket_end"));
    BOOST_REQUIRE_EQUAL(lastFinalizedBucketEnd * 1000,
                        static_cast<ml::core_t::TTime>(
                            flush["last_finalized_bucket_end"].GetInt64()));
}

BOOST_AUTO_TEST_CASE(testWriteCategoryDefinition) {
    ml::api::CGlobalCategoryId categoryId{42};
    std::string terms("foo bar");
    std::string regex(".*?foo.+?bar.*");
    std::size_t maxMatchingLength(132);
    ml::api::CJsonOutputWriter::TStrFSet examples;
    examples.insert("User foo failed to log in");
    examples.insert("User bar failed to log in");

    std::ostringstream sstream;

    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        writer.writeCategoryDefinition("", "", categoryId, terms, regex,
                                       maxMatchingLength, examples, 0, {});
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), arrayDoc.Size());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    arrayDoc.Accept(writer);
    LOG_DEBUG(<< "CategoryDefinition:\n" << strbuf.GetString());

    const rapidjson::Value& categoryWrapper = arrayDoc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(categoryWrapper.IsObject());
    BOOST_TEST_REQUIRE(categoryWrapper.HasMember("category_definition"));

    const rapidjson::Value& category = categoryWrapper["category_definition"];
    BOOST_TEST_REQUIRE(category.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", category["job_id"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("partition_field_name") == false);
    BOOST_TEST_REQUIRE(category.HasMember("partition_field_value") == false);
    BOOST_TEST_REQUIRE(category.IsObject());
    BOOST_TEST_REQUIRE(category.HasMember("category_id"));
    BOOST_REQUIRE_EQUAL(categoryId.globalId(), category["category_id"].GetInt());
    BOOST_TEST_REQUIRE(category.HasMember("terms"));
    BOOST_REQUIRE_EQUAL(terms, category["terms"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("regex"));
    BOOST_REQUIRE_EQUAL(regex, category["regex"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("max_matching_length"));
    BOOST_REQUIRE_EQUAL(maxMatchingLength,
                        static_cast<std::size_t>(category["max_matching_length"].GetInt()));
    BOOST_TEST_REQUIRE(category.HasMember("examples"));

    ml::api::CJsonOutputWriter::TStrFSet writtenExamplesSet;
    const rapidjson::Value& writtenExamples = category["examples"];
    for (rapidjson::SizeType i = 0; i < writtenExamples.Size(); i++) {
        writtenExamplesSet.insert(std::string(writtenExamples[i].GetString()));
    }
    BOOST_TEST_REQUIRE(writtenExamplesSet == examples);
}

BOOST_AUTO_TEST_CASE(testWritePerPartitionCategoryDefinition) {
    ml::api::CGlobalCategoryId categoryId{42};
    std::string terms("foo bar");
    std::string regex(".*?foo.+?bar.*");
    std::size_t maxMatchingLength(132);
    ml::api::CJsonOutputWriter::TStrFSet examples;
    examples.insert("User foo failed to log in");
    examples.insert("User bar failed to log in");

    std::ostringstream sstream;

    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        writer.writeCategoryDefinition("event.dataset", "elasticsearch", categoryId, terms,
                                       regex, maxMatchingLength, examples, 0, {});
    }

    rapidjson::Document arrayDoc;
    arrayDoc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    BOOST_TEST_REQUIRE(arrayDoc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(1), arrayDoc.Size());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    arrayDoc.Accept(writer);
    LOG_DEBUG(<< "CategoryDefinition:\n" << strbuf.GetString());

    const rapidjson::Value& categoryWrapper = arrayDoc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(categoryWrapper.IsObject());
    BOOST_TEST_REQUIRE(categoryWrapper.HasMember("category_definition"));

    const rapidjson::Value& category = categoryWrapper["category_definition"];
    BOOST_TEST_REQUIRE(category.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", category["job_id"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("partition_field_name"));
    BOOST_REQUIRE_EQUAL("event.dataset", category["partition_field_name"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("partition_field_value"));
    BOOST_REQUIRE_EQUAL("elasticsearch", category["partition_field_value"].GetString());
    BOOST_TEST_REQUIRE(category.IsObject());
    BOOST_TEST_REQUIRE(category.HasMember("category_id"));
    BOOST_REQUIRE_EQUAL(categoryId.globalId(), category["category_id"].GetInt());
    BOOST_TEST_REQUIRE(category.HasMember("terms"));
    BOOST_REQUIRE_EQUAL(terms, category["terms"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("regex"));
    BOOST_REQUIRE_EQUAL(regex, category["regex"].GetString());
    BOOST_TEST_REQUIRE(category.HasMember("max_matching_length"));
    BOOST_REQUIRE_EQUAL(maxMatchingLength,
                        static_cast<std::size_t>(category["max_matching_length"].GetInt()));
    BOOST_TEST_REQUIRE(category.HasMember("examples"));

    ml::api::CJsonOutputWriter::TStrFSet writtenExamplesSet;
    const rapidjson::Value& writtenExamples = category["examples"];
    for (rapidjson::SizeType i = 0; i < writtenExamples.Size(); i++) {
        writtenExamplesSet.insert(std::string(writtenExamples[i].GetString()));
    }
    BOOST_TEST_REQUIRE(writtenExamplesSet == examples);
}

BOOST_AUTO_TEST_CASE(testBucketWrite) {
    testBucketWriteHelper(false);
}

BOOST_AUTO_TEST_CASE(testBucketWriteInterim) {
    testBucketWriteHelper(true);
}

BOOST_AUTO_TEST_CASE(testLimitedRecordsWrite) {
    testLimitedRecordsWriteHelper(false);
}

BOOST_AUTO_TEST_CASE(testLimitedRecordsWriteInterim) {
    testLimitedRecordsWriteHelper(true);
}

BOOST_AUTO_TEST_CASE(testWriteInfluencers) {
    std::ostringstream sstream;

    {
        std::string user("user");
        std::string daisy("daisy");
        std::string jim("jim");

        ml::model::CHierarchicalResults::TNode node1 =
            createInfluencerNode(user, daisy, 0.5, 10.0);
        ml::model::CHierarchicalResults::TNode node2 =
            createInfluencerNode(user, jim, 0.9, 100.0);

        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(42), node1, false));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(42), node2, false));

        // Finished adding results
        BOOST_TEST_REQUIRE(writer.endOutputBatch(true, 1U));
    }

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    // Debug print record
    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    doc.Accept(writer);
    LOG_DEBUG(<< "influencers:\n" << strbuf.GetString());

    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), doc.Size());

    const rapidjson::Value& influencers = doc[rapidjson::SizeType(0)]["influencers"];
    BOOST_TEST_REQUIRE(influencers.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), influencers.Size());

    const rapidjson::Value& influencer = influencers[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(influencer.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", influencer["job_id"].GetString());
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.5, influencer["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(
        10.0, influencer["initial_influencer_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(influencer.HasMember("influencer_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, influencer["influencer_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("user", influencer["influencer_field_name"].GetString());
    BOOST_REQUIRE_EQUAL("daisy", influencer["influencer_field_value"].GetString());
    BOOST_REQUIRE_EQUAL(42000, influencer["timestamp"].GetInt());
    BOOST_TEST_REQUIRE(influencer["is_interim"].GetBool());
    BOOST_TEST_REQUIRE(influencer.HasMember("bucket_span"));

    const rapidjson::Value& influencer2 = influencers[rapidjson::SizeType(1)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.9, influencer2["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(
        100.0, influencer2["initial_influencer_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(influencer2.HasMember("influencer_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(100.0, influencer2["influencer_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("user", influencer2["influencer_field_name"].GetString());
    BOOST_REQUIRE_EQUAL("jim", influencer2["influencer_field_value"].GetString());
    BOOST_REQUIRE_EQUAL(42000, influencer2["timestamp"].GetInt());
    BOOST_TEST_REQUIRE(influencer2["is_interim"].GetBool());
    BOOST_TEST_REQUIRE(influencer2.HasMember("bucket_span"));

    const rapidjson::Value& bucket = doc[rapidjson::SizeType(1)]["bucket"];
    BOOST_TEST_REQUIRE(bucket.HasMember("influencers") == false);
}

BOOST_AUTO_TEST_CASE(testWriteInfluencersWithLimit) {
    std::ostringstream sstream;

    {
        std::string user("user");
        std::string computer("computer");
        std::string monitor("monitor");
        std::string daisy("daisy");
        std::string jim("jim");
        std::string bob("bob");
        std::string laptop("laptop");

        ml::model::CHierarchicalResults::TNode node1 =
            createInfluencerNode(user, daisy, 0.5, 10.0);
        ml::model::CHierarchicalResults::TNode node2 =
            createInfluencerNode(user, jim, 0.9, 100.0);
        ml::model::CHierarchicalResults::TNode node3 =
            createInfluencerNode(user, bob, 0.3, 9.0);
        ml::model::CHierarchicalResults::TNode node4 =
            createInfluencerNode(computer, laptop, 0.3, 12.0);

        ml::model::CHierarchicalResults::TNode bnode1 =
            createBucketInfluencerNode(user, 0.5, 10.0, 1.0);
        ml::model::CHierarchicalResults::TNode bnode2 =
            createBucketInfluencerNode(computer, 0.9, 100.0, 10.0);
        ml::model::CHierarchicalResults::TNode bnode3 =
            createBucketInfluencerNode(monitor, 0.3, 9.0, 0.9);

        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);
        writer.limitNumberRecords(2);

        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), node1, false));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), node2, false));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), node3, false));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), node4, false));

        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), bnode1, true));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), bnode2, true));
        BOOST_TEST_REQUIRE(writer.acceptInfluencer(ml::core_t::TTime(0), bnode3, true));

        // can't add a bucket influencer unless a result has been added
        std::string pfn("partition_field_name");
        std::string pfv("partition_field_value");
        std::string bfn("by_field_name");
        std::string bfv("by_field_value");
        std::string fun("function");
        std::string fund("function_description");
        std::string fn("field_name");
        std::string emptyStr;
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;
        ml::api::CHierarchicalResultsWriter::SResults result(
            ml::api::CHierarchicalResultsWriter::E_Result, pfn, pfv, bfn, bfv,
            emptyStr, 0, fun, fund, 42.0, 79, TDouble1Vec(1, 6953.0),
            TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.1, -5.0, fn, influences, false,
            true, 1, 100, EMPTY_STRING_LIST);

        BOOST_TEST_REQUIRE(writer.acceptResult(result));

        writer.acceptBucketTimeInfluencer(ml::core_t::TTime(0), 0.6, 1.0, 10.0);

        // Finished adding results
        BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
    }

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    rapidjson::StringBuffer strbuf;
    using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    TStringBufferPrettyWriter writer(strbuf);
    doc.Accept(writer);

    LOG_DEBUG(<< "limited write influencers:\n" << strbuf.GetString());

    const rapidjson::Value& influencers = doc[rapidjson::SizeType(1)]["influencers"];
    BOOST_TEST_REQUIRE(influencers.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), influencers.Size());

    const rapidjson::Value& influencer = influencers[rapidjson::SizeType(0)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.9, influencer["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(
        100.0, influencer["initial_influencer_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(influencer.HasMember("influencer_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(100.0, influencer["influencer_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("user", influencer["influencer_field_name"].GetString());
    BOOST_REQUIRE_EQUAL("jim", influencer["influencer_field_value"].GetString());
    BOOST_TEST_REQUIRE(influencer.HasMember("bucket_span"));

    const rapidjson::Value& influencer2 = influencers[rapidjson::SizeType(1)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.3, influencer2["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(
        12.0, influencer2["initial_influencer_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(influencer2.HasMember("influencer_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(12.0, influencer2["influencer_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("computer", influencer2["influencer_field_name"].GetString());
    BOOST_REQUIRE_EQUAL("laptop", influencer2["influencer_field_value"].GetString());
    BOOST_TEST_REQUIRE(influencer2.HasMember("bucket_span"));

    // bucket influencers
    const rapidjson::Value& bucketResult = doc[rapidjson::SizeType(2)]["bucket"];
    BOOST_TEST_REQUIRE(bucketResult.HasMember("bucket_influencers"));
    const rapidjson::Value& bucketInfluencers = bucketResult["bucket_influencers"];
    BOOST_TEST_REQUIRE(bucketInfluencers.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(3), bucketInfluencers.Size());

    const rapidjson::Value& binf = bucketInfluencers[rapidjson::SizeType(0)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.9, binf["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(100.0, binf["initial_anomaly_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(binf.HasMember("anomaly_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(100.0, binf["anomaly_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("computer", binf["influencer_field_name"].GetString());
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, binf["raw_anomaly_score"].GetDouble(), 0.001);

    const rapidjson::Value& binf2 = bucketInfluencers[rapidjson::SizeType(1)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.5, binf2["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, binf2["initial_anomaly_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(binf2.HasMember("anomaly_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, binf2["anomaly_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("user", binf2["influencer_field_name"].GetString());
    BOOST_REQUIRE_CLOSE_ABSOLUTE(1.0, binf2["raw_anomaly_score"].GetDouble(), 0.001);

    const rapidjson::Value& binf3 = bucketInfluencers[rapidjson::SizeType(2)];
    BOOST_REQUIRE_CLOSE_ABSOLUTE(0.6, binf3["probability"].GetDouble(), 0.001);
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, binf3["initial_anomaly_score"].GetDouble(), 0.001);
    BOOST_TEST_REQUIRE(binf3.HasMember("anomaly_score"));
    BOOST_REQUIRE_CLOSE_ABSOLUTE(10.0, binf3["anomaly_score"].GetDouble(), 0.001);
    BOOST_REQUIRE_EQUAL("bucket_time", binf3["influencer_field_name"].GetString());
    BOOST_REQUIRE_CLOSE_ABSOLUTE(1.0, binf3["raw_anomaly_score"].GetDouble(), 0.001);
}

BOOST_AUTO_TEST_CASE(testWriteWithInfluences) {
    std::ostringstream sstream;

    {
        std::string partitionFieldName("tfn");
        std::string partitionFieldValue("tfv");
        std::string overFieldName("pfn");
        std::string overFieldValue("pfv");
        std::string byFieldName("airline");
        std::string byFieldValue("GAL");
        std::string fieldName("responsetime");
        std::string function("mean");
        std::string functionDescription("mean(responsetime)");
        std::string emptyString;
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;

        std::string user("user");
        std::string dave("dave");
        std::string jo("jo");
        std::string cat("cat");
        std::string host("host");
        std::string localhost("localhost");
        std::string webserver("web-server");

        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr field1 =
            ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr(
                ml::model::CStringStore::names().get(user),
                ml::model::CStringStore::names().get(dave));
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr field2 =
            ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr(
                ml::model::CStringStore::names().get(user),
                ml::model::CStringStore::names().get(cat));
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr field3 =
            ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr(
                ml::model::CStringStore::names().get(user),
                ml::model::CStringStore::names().get(jo));

        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr hostField1 =
            ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr(
                ml::model::CStringStore::names().get(host),
                ml::model::CStringStore::names().get(localhost));
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr hostField2 =
            ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPr(
                ml::model::CStringStore::names().get(host),
                ml::model::CStringStore::names().get(webserver));

        influences.push_back(ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePr(
            field1, 0.4));
        influences.push_back(ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePr(
            field2, 1.0));
        influences.push_back(ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePr(
            hostField1, 0.7));
        influences.push_back(ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePr(
            field3, 0.1));
        influences.push_back(ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePr(
            hostField2, 0.8));

        // The output writer won't close the JSON structures until is is destroyed

        ml::api::CHierarchicalResultsWriter::SResults result(
            ml::api::CHierarchicalResultsWriter::E_Result, partitionFieldName,
            partitionFieldValue, byFieldName, byFieldValue, emptyString, 1,
            function, functionDescription, 42.0, 79, TDouble1Vec(1, 6953.0),
            TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.1, -5.0, fieldName, influences,
            false, true, 1, 100, EMPTY_STRING_LIST);

        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);
        BOOST_TEST_REQUIRE(writer.acceptResult(result));

        // Finished adding results
        BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
    }

    rapidjson::Document doc;
    std::string out = sstream.str();
    doc.Parse<rapidjson::kParseDefaultFlags>(out);

    // Debug print record
    {
        rapidjson::StringBuffer strbuf;
        using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
        TStringBufferPrettyWriter writer(strbuf);
        doc.Accept(writer);
        LOG_DEBUG(<< "Results:\n" << strbuf.GetString());
    }

    BOOST_TEST_REQUIRE(doc[rapidjson::SizeType(1)].HasMember("bucket"));
    const rapidjson::Value& bucket = doc[rapidjson::SizeType(1)]["bucket"];
    BOOST_TEST_REQUIRE(bucket.HasMember("records") == false);

    BOOST_TEST_REQUIRE(doc[rapidjson::SizeType(0)].HasMember("records"));
    const rapidjson::Value& records = doc[rapidjson::SizeType(0)]["records"];

    BOOST_TEST_REQUIRE(records[rapidjson::SizeType(0)].HasMember("influencers"));
    const rapidjson::Value& influences = records[rapidjson::SizeType(0)]["influencers"];

    BOOST_TEST_REQUIRE(influences.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), influences.Size());

    {
        const rapidjson::Value& influence = influences[rapidjson::SizeType(0)];
        BOOST_TEST_REQUIRE(influence.HasMember("influencer_field_name"));
        BOOST_REQUIRE_EQUAL("host", influence["influencer_field_name"].GetString());
        BOOST_TEST_REQUIRE(influence.HasMember("influencer_field_values"));
        const rapidjson::Value& influencerFieldValues = influence["influencer_field_values"];
        BOOST_TEST_REQUIRE(influencerFieldValues.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), influencerFieldValues.Size());

        // Check influencers are ordered
        BOOST_REQUIRE_EQUAL("web-server",
                            influencerFieldValues[rapidjson::SizeType(0)].GetString());
        BOOST_REQUIRE_EQUAL(
            "localhost", influencerFieldValues[rapidjson::SizeType(1)].GetString());
    }
    {
        const rapidjson::Value& influence = influences[rapidjson::SizeType(1)];
        BOOST_TEST_REQUIRE(influence.HasMember("influencer_field_name"));
        BOOST_REQUIRE_EQUAL("user", influence["influencer_field_name"].GetString());
        BOOST_TEST_REQUIRE(influence.HasMember("influencer_field_values"));
        const rapidjson::Value& influencerFieldValues = influence["influencer_field_values"];
        BOOST_TEST_REQUIRE(influencerFieldValues.IsArray());
        BOOST_REQUIRE_EQUAL(rapidjson::SizeType(3), influencerFieldValues.Size());

        // Check influencers are ordered
        BOOST_REQUIRE_EQUAL("cat", influencerFieldValues[rapidjson::SizeType(0)].GetString());
        BOOST_REQUIRE_EQUAL(
            "dave", influencerFieldValues[rapidjson::SizeType(1)].GetString());
        BOOST_REQUIRE_EQUAL("jo", influencerFieldValues[rapidjson::SizeType(2)].GetString());
    }
}

BOOST_AUTO_TEST_CASE(testPersistNormalizer) {
    ml::model::CAnomalyDetectorModelConfig modelConfig =
        ml::model::CAnomalyDetectorModelConfig::defaultConfig();

    std::ostringstream sstream;
    ml::core_t::TTime persistTime(1);
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        ml::model::CHierarchicalResultsNormalizer normalizer(modelConfig);
        writer.persistNormalizer(normalizer, persistTime);
        writer.finalise();
    }

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    BOOST_TEST_REQUIRE(persistTime <= ml::core::CTimeUtils::now());
    BOOST_TEST_REQUIRE(persistTime > ml::core::CTimeUtils::now() - 10);

    BOOST_TEST_REQUIRE(doc.IsArray());

    const rapidjson::Value& quantileWrapper = doc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(quantileWrapper.HasMember("quantiles"));
    const rapidjson::Value& quantileState = quantileWrapper["quantiles"];
    BOOST_TEST_REQUIRE(quantileState.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", quantileState["job_id"].GetString());
    BOOST_TEST_REQUIRE(quantileState.HasMember("quantile_state"));
    BOOST_TEST_REQUIRE(quantileState.HasMember("timestamp"));
}

BOOST_AUTO_TEST_CASE(testReportMemoryUsage) {
    std::ostringstream sstream;
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        ml::model::CResourceMonitor::SModelSizeStats resourceUsage;
        resourceUsage.s_Usage = 1;
        resourceUsage.s_AdjustedUsage = 2;
        resourceUsage.s_PeakUsage = 3;
        resourceUsage.s_AdjustedPeakUsage = 4;
        resourceUsage.s_ByFields = 5;
        resourceUsage.s_PartitionFields = 6;
        resourceUsage.s_OverFields = 7;
        resourceUsage.s_AllocationFailures = 8;
        resourceUsage.s_MemoryStatus = ml::model_t::E_MemoryStatusHardLimit;
        resourceUsage.s_AssignmentMemoryBasis = ml::model_t::E_AssignmentBasisCurrentModelBytes;
        resourceUsage.s_BucketStartTime = 9;
        resourceUsage.s_BytesExceeded = 10;
        resourceUsage.s_BytesMemoryLimit = 11;
        resourceUsage.s_OverallCategorizerStats.s_CategorizedMessages = 12;
        resourceUsage.s_OverallCategorizerStats.s_TotalCategories = 13;
        resourceUsage.s_OverallCategorizerStats.s_FrequentCategories = 14;
        resourceUsage.s_OverallCategorizerStats.s_RareCategories = 15;
        resourceUsage.s_OverallCategorizerStats.s_DeadCategories = 16;
        resourceUsage.s_OverallCategorizerStats.s_MemoryCategorizationFailures = 17;
        resourceUsage.s_OverallCategorizerStats.s_CategorizationStatus =
            ml::model_t::E_CategorizationStatusWarn;

        writer.reportMemoryUsage(resourceUsage);
        writer.endOutputBatch(false, 1ul);
    }

    LOG_DEBUG(<< sstream.str());

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    const rapidjson::Value& resourceWrapper = doc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(resourceWrapper.HasMember("model_size_stats"));
    const rapidjson::Value& sizeStats = resourceWrapper["model_size_stats"];

    BOOST_TEST_REQUIRE(sizeStats.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", sizeStats["job_id"].GetString());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("model_bytes"));
    BOOST_REQUIRE_EQUAL(2, sizeStats["model_bytes"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("peak_model_bytes"));
    BOOST_REQUIRE_EQUAL(4, sizeStats["peak_model_bytes"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("total_by_field_count"));
    BOOST_REQUIRE_EQUAL(5, sizeStats["total_by_field_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("total_partition_field_count"));
    BOOST_REQUIRE_EQUAL(6, sizeStats["total_partition_field_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("total_over_field_count"));
    BOOST_REQUIRE_EQUAL(7, sizeStats["total_over_field_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("bucket_allocation_failures_count"));
    BOOST_REQUIRE_EQUAL(8, sizeStats["bucket_allocation_failures_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("timestamp"));
    BOOST_REQUIRE_EQUAL(9000, sizeStats["timestamp"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("memory_status"));
    BOOST_REQUIRE_EQUAL("hard_limit", sizeStats["memory_status"].GetString());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("assignment_memory_basis"));
    BOOST_REQUIRE_EQUAL("current_model_bytes",
                        sizeStats["assignment_memory_basis"].GetString());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("log_time"));
    std::int64_t nowMs{ml::core::CTimeUtils::nowMs()};
    BOOST_TEST_REQUIRE(nowMs >= sizeStats["log_time"].GetInt64());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("model_bytes_exceeded"));
    BOOST_REQUIRE_EQUAL(10, sizeStats["model_bytes_exceeded"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("model_bytes_memory_limit"));
    BOOST_REQUIRE_EQUAL(11, sizeStats["model_bytes_memory_limit"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("categorized_doc_count"));
    BOOST_REQUIRE_EQUAL(12, sizeStats["categorized_doc_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("total_category_count"));
    BOOST_REQUIRE_EQUAL(13, sizeStats["total_category_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("frequent_category_count"));
    BOOST_REQUIRE_EQUAL(14, sizeStats["frequent_category_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("rare_category_count"));
    BOOST_REQUIRE_EQUAL(15, sizeStats["rare_category_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("dead_category_count"));
    BOOST_REQUIRE_EQUAL(16, sizeStats["dead_category_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("failed_category_count"));
    BOOST_REQUIRE_EQUAL(17, sizeStats["failed_category_count"].GetInt());
    BOOST_TEST_REQUIRE(sizeStats.HasMember("categorization_status"));
    BOOST_REQUIRE_EQUAL("warn", sizeStats["categorization_status"].GetString());
}

BOOST_AUTO_TEST_CASE(testWriteCategorizerStats) {
    std::ostringstream sstream;
    {
        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        ml::model::SCategorizerStats categorizerStats;
        categorizerStats.s_CategorizedMessages = 1;
        categorizerStats.s_TotalCategories = 2;
        categorizerStats.s_FrequentCategories = 3;
        categorizerStats.s_RareCategories = 4;
        categorizerStats.s_DeadCategories = 5;
        categorizerStats.s_MemoryCategorizationFailures = 6;
        categorizerStats.s_CategorizationStatus = ml::model_t::E_CategorizationStatusOk;

        writer.writeCategorizerStats("foo", "bar", categorizerStats, 7);
        writer.endOutputBatch(false, 1ul);
    }

    LOG_DEBUG(<< sstream.str());

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(sstream.str().c_str());

    const rapidjson::Value& resourceWrapper = doc[rapidjson::SizeType(0)];
    BOOST_TEST_REQUIRE(resourceWrapper.HasMember("categorizer_stats"));
    const rapidjson::Value& categorizerStats = resourceWrapper["categorizer_stats"];

    BOOST_TEST_REQUIRE(categorizerStats.HasMember("job_id"));
    BOOST_REQUIRE_EQUAL("job", categorizerStats["job_id"].GetString());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("partition_field_name"));
    BOOST_REQUIRE_EQUAL("foo", categorizerStats["partition_field_name"].GetString());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("partition_field_value"));
    BOOST_REQUIRE_EQUAL("bar", categorizerStats["partition_field_value"].GetString());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("categorized_doc_count"));
    BOOST_REQUIRE_EQUAL(1, categorizerStats["categorized_doc_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("total_category_count"));
    BOOST_REQUIRE_EQUAL(2, categorizerStats["total_category_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("frequent_category_count"));
    BOOST_REQUIRE_EQUAL(3, categorizerStats["frequent_category_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("rare_category_count"));
    BOOST_REQUIRE_EQUAL(4, categorizerStats["rare_category_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("dead_category_count"));
    BOOST_REQUIRE_EQUAL(5, categorizerStats["dead_category_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("failed_category_count"));
    BOOST_REQUIRE_EQUAL(6, categorizerStats["failed_category_count"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("categorization_status"));
    BOOST_REQUIRE_EQUAL("ok", categorizerStats["categorization_status"].GetString());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("categorization_status"));
    BOOST_REQUIRE_EQUAL("ok", categorizerStats["categorization_status"].GetString());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("timestamp"));
    BOOST_REQUIRE_EQUAL(7000, categorizerStats["timestamp"].GetInt());
    BOOST_TEST_REQUIRE(categorizerStats.HasMember("log_time"));
    std::int64_t nowMs{ml::core::CTimeUtils::nowMs()};
    BOOST_TEST_REQUIRE(nowMs >= categorizerStats["log_time"].GetInt64());
}

BOOST_AUTO_TEST_CASE(testWriteScheduledEvent) {
    std::ostringstream sstream;

    {
        std::string partitionFieldName("tfn");
        std::string partitionFieldValue("tfv");
        std::string byFieldName("airline");
        std::string byFieldValue("GAL");
        std::string fieldName("responsetime");
        std::string function("mean");
        std::string functionDescription("mean(responsetime)");
        std::string emptyString;
        ml::api::CHierarchicalResultsWriter::TStoredStringPtrStoredStringPtrPrDoublePrVec influences;

        ml::core::CJsonOutputStreamWrapper outputStream(sstream);
        ml::api::CJsonOutputWriter writer("job", outputStream);

        // This result has no scheduled events
        ml::api::CHierarchicalResultsWriter::SResults result(
            ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
            partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
            emptyString, 100, function, functionDescription, 42.0, 79,
            TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.1,
            -5.0, fieldName, influences, false, true, 1, 100, EMPTY_STRING_LIST);
        BOOST_TEST_REQUIRE(writer.acceptResult(result));

        // This result has 2 scheduled events
        std::vector<std::string> eventDescriptions{"event-foo", "event-bar"};
        ml::api::CHierarchicalResultsWriter::SResults result2(
            ml::api::CHierarchicalResultsWriter::E_SimpleCountResult,
            partitionFieldName, partitionFieldValue, byFieldName, byFieldValue,
            emptyString, 200, function, functionDescription, 42.0, 79,
            TDouble1Vec(1, 6953.0), TDouble1Vec(1, 10090.0), 0.0, 0.1, 0.1,
            -5.0, fieldName, influences, false, true, 1, 100, eventDescriptions);

        BOOST_TEST_REQUIRE(writer.acceptResult(result2));
        BOOST_TEST_REQUIRE(writer.endOutputBatch(false, 1U));
    }

    rapidjson::Document doc;
    std::string out = sstream.str();
    doc.Parse<rapidjson::kParseDefaultFlags>(out);

    // Debug print record
    {
        rapidjson::StringBuffer strbuf;
        using TStringBufferPrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
        TStringBufferPrettyWriter writer(strbuf);
        doc.Accept(writer);
        LOG_DEBUG(<< "Results:\n" << strbuf.GetString());
    }

    BOOST_TEST_REQUIRE(doc.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), doc.Size());
    // the first bucket has no events
    const rapidjson::Value& bucket = doc[rapidjson::SizeType(1)]["bucket"];
    BOOST_TEST_REQUIRE(bucket.HasMember("scheduled_event") == false);

    const rapidjson::Value& bucketWithEvents = doc[rapidjson::SizeType(1)]["bucket"];
    BOOST_TEST_REQUIRE(bucketWithEvents.HasMember("scheduled_events"));
    const rapidjson::Value& events = bucketWithEvents["scheduled_events"];
    BOOST_TEST_REQUIRE(events.IsArray());
    BOOST_REQUIRE_EQUAL(rapidjson::SizeType(2), events.Size());
    BOOST_REQUIRE_EQUAL("event-foo", events[rapidjson::SizeType(0)].GetString());
    BOOST_REQUIRE_EQUAL("event-bar", events[rapidjson::SizeType(1)].GetString());
}

BOOST_AUTO_TEST_CASE(testThroughputWithScopedAllocator) {
    testThroughputHelper(true);
}

BOOST_AUTO_TEST_CASE(testThroughputWithoutScopedAllocator) {
    testThroughputHelper(false);
}

BOOST_AUTO_TEST_SUITE_END()
