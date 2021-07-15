#pragma once

namespace gpc {
	class GPCTracker {

	};

	class GPCOBJTracker : public GPCTracker {

	};

	class GPCSwapChainTracker : public GPCOBJTracker {

	};

    class GPCDeviceTracker : public GPCOBJTracker {

    };

	class GPCCommandQueueTracker : public GPCOBJTracker {

	};

	class GPCCommandListTracker : public GPCOBJTracker {

	};

	class GPCPipelineStateObjectTracker : public GPCOBJTracker {

	};

	class GPCRootSignatureTracker : public GPCOBJTracker {

	};

	class GPCTrackerManager {

	};
}