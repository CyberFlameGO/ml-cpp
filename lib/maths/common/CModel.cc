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

#include <maths/common/CModel.h>
#include <maths/common/CModelDetail.h>

#include <core/CLogger.h>
#include <core/Constants.h>

#include <maths/common/CTools.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace ml {
namespace maths {
namespace common {
namespace {

const std::string EMPTY_STRING;
const double EFFECTIVE_COUNT[]{1.0,  0.8,  0.7,  0.65, 0.6,
                               0.57, 0.54, 0.52, 0.51};

//! Get the parameters for the stub model.
CModelParams stubParameters() {
    return CModelParams{
        0, 1.0, 0.0, 0.0, 6 * core::constants::HOUR, core::constants::DAY};
}
}

//////// CModelParams ////////

CModelParams::CModelParams(core_t::TTime bucketLength,
                           double learnRate,
                           double decayRate,
                           double minimumSeasonalVarianceScale,
                           core_t::TTime minimumTimeToDetectChange,
                           core_t::TTime maximumTimeToTestForChange)
    : m_BucketLength(bucketLength), m_LearnRate(learnRate), m_DecayRate(decayRate),
      m_MinimumSeasonalVarianceScale(minimumSeasonalVarianceScale),
      m_MinimumTimeToDetectChange(std::max(minimumTimeToDetectChange, 6 * bucketLength)),
      m_MaximumTimeToTestForChange(std::max(maximumTimeToTestForChange, 12 * bucketLength)) {
}

core_t::TTime CModelParams::bucketLength() const {
    return m_BucketLength;
}

double CModelParams::learnRate() const {
    return m_LearnRate;
}

double CModelParams::decayRate() const {
    return m_DecayRate;
}

double CModelParams::averagingDecayRate() const {
    return 5.0 * m_DecayRate;
}

double CModelParams::minimumSeasonalVarianceScale() const {
    return m_MinimumSeasonalVarianceScale;
}

bool CModelParams::testForChange(core_t::TTime changeInterval) const {
    return changeInterval >= std::max(3 * m_BucketLength, core::constants::HOUR);
}

core_t::TTime CModelParams::minimumTimeToDetectChange() const {
    return m_MinimumTimeToDetectChange;
}

core_t::TTime CModelParams::maximumTimeToTestForChange() const {
    return m_MaximumTimeToTestForChange;
}

//////// CModelAddSamplesParams ////////

CModelAddSamplesParams& CModelAddSamplesParams::isInteger(bool isInteger) {
    m_Type = isInteger ? maths_t::E_IntegerData : maths_t::E_ContinuousData;
    return *this;
}

maths_t::EDataType CModelAddSamplesParams::type() const {
    return m_Type;
}

CModelAddSamplesParams& CModelAddSamplesParams::isNonNegative(bool isNonNegative) {
    m_IsNonNegative = isNonNegative;
    return *this;
}

bool CModelAddSamplesParams::isNonNegative() const {
    return m_IsNonNegative;
}

CModelAddSamplesParams& CModelAddSamplesParams::bucketOccupancy(double occupancy) {
    m_Occupancy = occupancy;
    return *this;
}

double CModelAddSamplesParams::bucketOccupancy() const {
    return m_Occupancy;
}

CModelAddSamplesParams& CModelAddSamplesParams::firstValueTime(core_t::TTime time) {
    m_FirstValueTime = time;
    return *this;
}

core_t::TTime CModelAddSamplesParams::firstValueTime() const {
    return m_FirstValueTime;
}

CModelAddSamplesParams& CModelAddSamplesParams::propagationInterval(double interval) {
    m_PropagationInterval = interval;
    return *this;
}

double CModelAddSamplesParams::propagationInterval() const {
    return m_PropagationInterval;
}

CModelAddSamplesParams&
CModelAddSamplesParams::trendWeights(const TDouble2VecWeightsAryVec& weights) {
    m_TrendWeights = &weights;
    return *this;
}

const CModelAddSamplesParams::TDouble2VecWeightsAryVec&
CModelAddSamplesParams::trendWeights() const {
    return *m_TrendWeights;
}

CModelAddSamplesParams&
CModelAddSamplesParams::priorWeights(const TDouble2VecWeightsAryVec& weights) {
    m_PriorWeights = &weights;
    return *this;
}

const CModelAddSamplesParams::TDouble2VecWeightsAryVec&
CModelAddSamplesParams::priorWeights() const {
    return *m_PriorWeights;
}

CModelAddSamplesParams&
CModelAddSamplesParams::annotationCallback(const maths_t::TModelAnnotationCallback& modelAnnotationCallback) {
    m_ModelAnnotationCallback = modelAnnotationCallback;
    return *this;
}

const maths_t::TModelAnnotationCallback& CModelAddSamplesParams::annotationCallback() const {
    return m_ModelAnnotationCallback;
}

//////// CModelProbabilityParams ////////

CModelProbabilityParams::CModelProbabilityParams()
    : m_SeasonalConfidenceInterval{DEFAULT_SEASONAL_CONFIDENCE_INTERVAL} {
}

CModelProbabilityParams&
CModelProbabilityParams::addCalculation(maths_t::EProbabilityCalculation calculation) {
    m_Calculations.push_back(calculation);
    return *this;
}

std::size_t CModelProbabilityParams::calculations() const {
    return m_Calculations.size();
}

maths_t::EProbabilityCalculation CModelProbabilityParams::calculation(std::size_t i) const {
    return m_Calculations.size() == 1 ? m_Calculations[0] : m_Calculations[i];
}

CModelProbabilityParams& CModelProbabilityParams::seasonalConfidenceInterval(double confidence) {
    m_SeasonalConfidenceInterval = confidence;
    return *this;
}

double CModelProbabilityParams::seasonalConfidenceInterval() const {
    return m_SeasonalConfidenceInterval;
}

CModelProbabilityParams&
CModelProbabilityParams::addWeights(const TDouble2VecWeightsAry& weights) {
    m_Weights.push_back(weights);
    return *this;
}

CModelProbabilityParams&
CModelProbabilityParams::weights(const TDouble2VecWeightsAry1Vec& weights) {
    m_Weights = weights;
    return *this;
}

const CModelProbabilityParams::TDouble2VecWeightsAry1Vec&
CModelProbabilityParams::weights() const {
    return m_Weights;
}

CModelProbabilityParams::TDouble2VecWeightsAry1Vec& CModelProbabilityParams::weights() {
    return m_Weights;
}

CModelProbabilityParams& CModelProbabilityParams::addCoordinate(std::size_t coordinate) {
    m_Coordinates.push_back(coordinate);
    return *this;
}

const CModelProbabilityParams::TSize2Vec& CModelProbabilityParams::coordinates() const {
    return m_Coordinates;
}

CModelProbabilityParams& CModelProbabilityParams::mostAnomalousCorrelate(std::size_t correlate) {
    m_MostAnomalousCorrelate.emplace(correlate);
    return *this;
}

CModelProbabilityParams::TOptionalSize CModelProbabilityParams::mostAnomalousCorrelate() const {
    return m_MostAnomalousCorrelate;
}

CModelProbabilityParams& CModelProbabilityParams::useMultibucketFeatures(bool use) {
    m_UseMultibucketFeatures = use;
    return *this;
}

bool CModelProbabilityParams::useMultibucketFeatures() const {
    return m_UseMultibucketFeatures;
}

CModelProbabilityParams& CModelProbabilityParams::useAnomalyModel(bool use) {
    m_UseAnomalyModel = use;
    return *this;
}

bool CModelProbabilityParams::useAnomalyModel() const {
    return m_UseAnomalyModel;
}

CModelProbabilityParams& CModelProbabilityParams::initialCountWeight(double initialCountWeight) {
    m_InitialCountWeight = initialCountWeight;
    return *this;
}

double CModelProbabilityParams::initialCountWeight() const {
    return m_InitialCountWeight;
}

//////// SModelProbabilityResult::SFeatureProbability ////////

SModelProbabilityResult::SFeatureProbability::SFeatureProbability()
    : s_Label{E_UndefinedProbability} {
}

SModelProbabilityResult::SFeatureProbability::SFeatureProbability(EFeatureProbabilityLabel label,
                                                                  double probability)
    : s_Label{label}, s_Probability{probability} {
}

//////// CModel ////////

const double CModel::DEFAULT_BOUNDS_PERCENTILE{95.0};

CModel::CModel(const CModelParams& params) : m_Params(params) {
}

double CModel::effectiveCount(std::size_t n) {
    return n <= std::size(EFFECTIVE_COUNT) ? EFFECTIVE_COUNT[n - 1] : 0.5;
}

double CModel::emptyBucketWeight(double occupancy) {
    // We smoothly transition to ignoring empty buckets when the bucket
    // occupancy is less than 0.5.
    return common::CTools::truncate(2.0 * occupancy, 1e-6, 1.0);
}

const CModelParams& CModel::params() const {
    return m_Params;
}

CModelParams& CModel::params() {
    return m_Params;
}

bool CModel::shouldPersist() const {
    return true;
}

//////// CModelStub ////////

CModelStub::CModelStub() : CModel(stubParameters()) {
}

std::size_t CModelStub::identifier() const {
    return 0;
}

CModelStub* CModelStub::clone(std::size_t /*id*/) const {
    return new CModelStub(*this);
}

CModelStub* CModelStub::cloneForPersistence() const {
    return new CModelStub(*this);
}

CModelStub* CModelStub::cloneForForecast() const {
    return new CModelStub(*this);
}

bool CModelStub::isForecastPossible() const {
    return false;
}

void CModelStub::modelCorrelations(time_series::CTimeSeriesCorrelations& /*model*/) {
}

CModelStub::TSize2Vec1Vec CModelStub::correlates() const {
    return {};
}

CModelStub::TDouble2Vec CModelStub::mode(core_t::TTime /*time*/,
                                         const TDouble2VecWeightsAry& /*weights*/) const {
    return {};
}

CModelStub::TDouble2Vec1Vec
CModelStub::correlateModes(core_t::TTime /*time*/,
                           const TDouble2VecWeightsAry1Vec& /*weights*/) const {
    return {};
}

CModelStub::TDouble2Vec1Vec
CModelStub::residualModes(const TDouble2VecWeightsAry& /*weights*/) const {
    return {};
}

void CModelStub::addBucketValue(const TTimeDouble2VecSizeTrVec& /*value*/) {
}

CModelStub::EUpdateResult CModelStub::addSamples(const CModelAddSamplesParams& /*params*/,
                                                 TTimeDouble2VecSizeTrVec /*samples*/) {
    return E_Success;
}

void CModelStub::skipTime(core_t::TTime /*gap*/) {
}

void CModelStub::detrend(const TTime2Vec1Vec& /*time*/,
                         double /*confidenceInterval*/,
                         TDouble2Vec1Vec& /*value*/) const {
}

CModelStub::TDouble2Vec CModelStub::predict(core_t::TTime /*time*/,
                                            const TSizeDoublePr1Vec& /*correlated*/,
                                            TDouble2Vec /*hint*/) const {
    return {};
}

CModelStub::TDouble2Vec3Vec
CModelStub::confidenceInterval(core_t::TTime /*time*/,
                               double /*confidenceInterval*/,
                               const TDouble2VecWeightsAry& /*weights*/) const {
    return {};
}

bool CModelStub::forecast(core_t::TTime /*firstDataTime*/,
                          core_t::TTime /*lastDataTime*/,
                          core_t::TTime /*startTime*/,
                          core_t::TTime /*endTime*/,
                          double /*confidenceInterval*/,
                          const TDouble2Vec& /*minimum*/,
                          const TDouble2Vec& /*maximum*/,
                          const TForecastPushDatapointFunc& /*forecastPushDataPointFunc*/,
                          std::string& /*messageOut*/) {
    return true;
}

bool CModelStub::probability(const CModelProbabilityParams& /*params*/,
                             const TTime2Vec1Vec& /*time*/,
                             const TDouble2Vec1Vec& /*value*/,
                             SModelProbabilityResult& result) const {
    result = SModelProbabilityResult{};
    return true;
}

void CModelStub::countWeights(core_t::TTime /*time*/,
                              const TDouble2Vec& /*value*/,
                              double /*trendCountWeight*/,
                              double /*residualCountWeight*/,
                              double /*outlierWeightDerate*/,
                              double /*countVarianceScale*/,
                              TDouble2VecWeightsAry& /*trendWeights*/,
                              TDouble2VecWeightsAry& /*residualWeights*/) const {
}

void CModelStub::addCountWeights(core_t::TTime /*time*/,
                                 double /*trendCountWeight*/,
                                 double /*residualCountWeight*/,
                                 double /*countVarianceScale*/,
                                 TDouble2VecWeightsAry& /*trendWeights*/,
                                 TDouble2VecWeightsAry& /*residualWeights*/) const {
}

void CModelStub::seasonalWeight(double /*confidence*/,
                                core_t::TTime /*time*/,
                                TDouble2Vec& /*weight*/) const {
}

std::uint64_t CModelStub::checksum(std::uint64_t seed) const {
    return seed;
}

void CModelStub::debugMemoryUsage(const core::CMemoryUsage::TMemoryUsagePtr& /*mem*/) const {
}

std::size_t CModelStub::memoryUsage() const {
    return 0;
}

void CModelStub::acceptPersistInserter(core::CStatePersistInserter& /*inserter*/) const {
}

void CModelStub::persistModelsState(core::CStatePersistInserter& /*inserter*/) const {
}

maths_t::EDataType CModelStub::dataType() const {
    return maths_t::E_MixedData;
}

bool CModelStub::shouldPersist() const {
    return false;
}
}
}
}
