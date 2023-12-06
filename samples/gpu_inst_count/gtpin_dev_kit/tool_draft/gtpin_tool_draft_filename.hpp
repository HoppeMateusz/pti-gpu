#ifndef PLGG_GTPIN_TOOL_DRAFT_H
#define PLGG_GTPIN_TOOL_DRAFT_H

/// Uncomment PLGG_GTPIN_API_VERSION definition if tool requeres GTPin SDK with
/// specific API version. GTPin SDK version constant (GTPIN_API_VERSION) is used
/// during tool registration in case of undefined PLGG_GTPIN_API_VERSION
// #define PLGG_GTPIN_API_VERSION 1007

#include "prof_lib_gpu_gtpin.hpp"

/**
 * @file Header file for GTPin-based tool draft
 */

namespace gtpin_prof {

using namespace gtpin;

/**
 * RESULTS classes
 * Extend tool-specific delivered classes with fields specific for the profiler.
 * This classes will be passed to writer
 */
struct GtpinToolDraftResultData : public ResultData {
 public:
  using ResultData::ResultData;
  virtual ~GtpinToolDraftResultData() = default;
};

struct GtpinToolDraftInvocationData : public InvocationData {
 public:
  using InvocationData::InvocationData;
  virtual ~GtpinToolDraftInvocationData() = default;
};

struct GtpinToolDraftKernelData : public KernelData {
 public:
  using KernelData::KernelData;
  virtual ~GtpinToolDraftKernelData() = default;
};

struct GtpinToolDraftProfilerData : public ProfilerData {
 public:
  GtpinToolDraftProfilerData() { toolName = "gtpin_tool_draft"; }
  virtual ~GtpinToolDraftProfilerData() = default;
};

/**
 * Delivered class of GTPin buffer record class. This delivered class is a
 * undividable unit in GTPin profile buffer, it is used in instrumentation and
 * stores data per instrumentation point. This class defines the data layout in
 * instrumentation buffer.
 */
struct GtpinToolDraftRecord : public GTPinProfileRecord {};

/**
 * GtpinToolDraftKernel
 * Delivered class of GTPinProfileKernel base class that implements
 * tool-specific virtual functions
 */
class GtpinToolDraftKernel : public GTPinProfileKernel {
  using GTPinProfileKernel::GTPinProfileKernel;

 public:
  /**
   * Called during kernel build. The function should set up number of records,
   * and collect requered information about points of interests, that will be
   * used on instrumentation stage
   */
  PROF_STATUS AnalyzeKernel(IGtKernelInstrument& instrumentor) final;

  /**
   * Called during kernel build. The function should instrument each point of
   * interest with tool-specific instrumentation
   */
  PROF_STATUS Instrument(IGtKernelInstrument& instrumentor) final;

  /**
   * Called before kernel run. Initializes resulting data, set ups vector sizes,
   * sets base values of variables
   */
  PROF_STATUS InitResultData(std::shared_ptr<InvocationData> invocationData,
                             IGtKernelDispatch& dispatcher,
                             const GTPinKernelExecDesriptor& execDescr,
                             const std::shared_ptr<IToolFactory> factory) final;

  /**
   * Called after kernel complete during reading of the GTPin profiling buffer.
   * Function should accumulate date from record into profiling results data
   */
  PROF_STATUS Accumulate(std::shared_ptr<ResultData> profilingResult,
                         GTPinProfileRecord* record) final;

  /**
   * Called after all data from the buffer was read. Does post processing,
   * normalization. Optional function.
   */
  PROF_STATUS PostProcData(std::shared_ptr<InvocationData> invocationData) final;
};

/**
 * GTPin tool registerable class
 * Determ tool-specific GTPin knobs into SetGtpinKnobs function
 */
class GtpinToolDraft : public GTPinTool {
  using GTPinTool::GTPinTool;

 public:
  std::vector<const char*> SetGtpinKnobs() const final;
  const char* Name() const final { return "gtpin_tool_draft"; }
};

/**
 * GtpinToolDraftFactory determines tool-specific objects
 */
class GtpinToolDraftFactory : public IToolFactory {
 public:
  std::shared_ptr<GTPinProfileKernel> MakeKernel(IGtKernelInstrument& instrumentor,
                                                 std::shared_ptr<KernelData> kernelData) final;
  GTPinProfileRecord* MakeRecord() final;
  std::shared_ptr<ProfilerData> MakeProfilerData() final;
  std::shared_ptr<KernelData> MakeKernelData(IGtKernelInstrument& instrumentor) final;
  std::shared_ptr<InvocationData> MakeInvocationData(
      const GTPinKernelExecDesriptor& execDescr) final;
  std::shared_ptr<ResultData> MakeResultData() final;
};

/**
 * Profiller class implementation
 */
class GtpinToolDraftProfiler : public GTPinProfilerBase {
 public:
  GtpinToolDraftProfiler(
      const std::shared_ptr<GTPinDataWriterBase> writer = std::make_shared<DefaultGTPinWriter>(),
      const std::shared_ptr<GTPinFilterBase> filter = std::make_shared<DefaultGTPinFilter>()) {
    tool =
        std::make_shared<GtpinToolDraft>(std::make_shared<GtpinToolDraftFactory>(), writer, filter);
  }
};

}  // namespace gtpin_prof

#endif  // PLGG_GTPIN_TOOL_DRAFT_H
