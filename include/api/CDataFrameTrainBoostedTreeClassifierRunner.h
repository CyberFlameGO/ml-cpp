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

#ifndef INCLUDED_ml_api_CDataFrameTrainBoostedTreeClassifierRunner_h
#define INCLUDED_ml_api_CDataFrameTrainBoostedTreeClassifierRunner_h

#include <core/CSmallVectorFwd.h>

#include <api/CDataFrameTrainBoostedTreeRunner.h>
#include <api/CInferenceModelMetadata.h>
#include <api/ImportExport.h>

#include <rapidjson/fwd.h>

namespace ml {
namespace api {

//! \brief Runs boosted tree classification on a core::CDataFrame.
class API_EXPORT CDataFrameTrainBoostedTreeClassifierRunner final
    : public CDataFrameTrainBoostedTreeRunner {
public:
    using TDouble2Vec = core::CSmallVector<double, 2>;
    using TReadPredictionFunc = std::function<TDouble2Vec(const TRowRef&)>;
    using TReadClassScoresFunc = std::function<TDouble2Vec(const TRowRef&)>;

    enum EPredictionFieldType {
        E_PredictionFieldTypeString,
        E_PredictionFieldTypeInt,
        E_PredictionFieldTypeBool
    };

public:
    static const std::size_t MAX_NUMBER_CLASSES;
    static const std::string NUM_CLASSES;
    static const std::string NUM_TOP_CLASSES;
    static const std::string PREDICTION_FIELD_TYPE;
    static const std::string CLASS_ASSIGNMENT_OBJECTIVE;
    static const std::string CLASSES_FIELD_NAME;
    static const std::string CLASS_NAME_FIELD_NAME;
    static const TStrVec CLASS_ASSIGNMENT_OBJECTIVE_VALUES;
    static const std::string CLASSIFICATION_WEIGHTS;
    static const std::string CLASSIFICATION_WEIGHTS_CLASS;
    static const std::string CLASSIFICATION_WEIGHTS_WEIGHT;

public:
    //! Get the runner's configuration parameter reader.
    static const CDataFrameAnalysisConfigReader& parameterReader();

    //! This is not intended to be called directly: use CDataFrameTrainBoostedTreeClassifierRunnerFactory.
    CDataFrameTrainBoostedTreeClassifierRunner(const CDataFrameAnalysisSpecification& spec,
                                               const CDataFrameAnalysisParameters& parameters,
                                               TDataFrameUPtrTemporaryDirectoryPtrPr* frameAndDirectory);

    //! Write the prediction for \p row to \p writer.
    void writeOneRow(const core::CDataFrame& frame,
                     const TRowRef& row,
                     core::CRapidJsonConcurrentLineWriter& writer) const override;

    //! Write the prediction for \p row to \p writer.
    //!
    //! \note This is only intended to be called directly from unit tests.
    void writeOneRow(const core::CDataFrame& frame,
                     std::size_t columnHoldingDependentVariable,
                     const TReadPredictionFunc& readClassProbabilities,
                     const TReadClassScoresFunc& readClassScores,
                     const TRowRef& row,
                     core::CRapidJsonConcurrentLineWriter& writer,
                     maths::analytics::CTreeShapFeatureImportance* featureImportance = nullptr) const;

    //! \return A serialisable definition of the trained classification model.
    TInferenceModelDefinitionUPtr
    inferenceModelDefinition(const TStrVec& fieldNames,
                             const TStrVecVec& categoryNames) const override;

    //! \return A serialisable metadata of the trained regression model.
    const CInferenceModelMetadata* inferenceModelMetadata() const override;

private:
    static TLossFunctionUPtr loss(std::size_t numberClasses);

    void validate(const core::CDataFrame& frame,
                  std::size_t dependentVariableColumn) const override;

    void writePredictedCategoryValue(const std::string& categoryValue,
                                     core::CRapidJsonConcurrentLineWriter& writer) const;

private:
    std::size_t m_NumClasses;
    std::ptrdiff_t m_NumTopClasses;
    EPredictionFieldType m_PredictionFieldType;
    mutable CInferenceModelMetadata m_InferenceModelMetadata;
};

//! \brief Makes a core::CDataFrame boosted tree classification runner.
class API_EXPORT CDataFrameTrainBoostedTreeClassifierRunnerFactory final
    : public CDataFrameAnalysisRunnerFactory {
public:
    static const std::string NAME;

public:
    const std::string& name() const override;

private:
    TRunnerUPtr makeImpl(const CDataFrameAnalysisSpecification& spec,
                         TDataFrameUPtrTemporaryDirectoryPtrPr* frameAndDirectory) const override;
    TRunnerUPtr makeImpl(const CDataFrameAnalysisSpecification& spec,
                         const rapidjson::Value& jsonParameters,
                         TDataFrameUPtrTemporaryDirectoryPtrPr* frameAndDirectory) const override;
};
}
}

#endif // INCLUDED_ml_api_CDataFrameTrainBoostedTreeClassifierRunner_h
