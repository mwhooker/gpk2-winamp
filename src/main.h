__interface MetaRetriever;

class WAMetaRetriever : public MetaRetriever
{
	//class CWinampController *Controller_;
public:
	WAMetaRetriever();//CWinampController *controller);
	bool CopyCurrentMetaInfo(MetaInfo& Meta);
};
