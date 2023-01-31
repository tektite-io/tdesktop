/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

class History;
class HistoryItem;
struct LanguageId;

namespace HistoryView {

class Element;

class TranslateTracker final {
public:
	explicit TranslateTracker(not_null<History*> history);
	~TranslateTracker();

	[[nodiscard]] bool enoughForRecognition() const;
	void startBunch();
	void add(not_null<Element*> view, LanguageId translatedTo);
	void add(not_null<HistoryItem*> item, LanguageId translatedTo);
	void finishBunch();

	[[nodiscard]] rpl::producer<bool> trackingLanguage() const;

private:
	using MaybeLanguageId = std::variant<QString, LanguageId>;
	struct ItemForRecognize;
	struct ItemToRequest {
		int length = 0;
	};

	void setup();
	void add(
		not_null<HistoryItem*> item,
		LanguageId translatedTo,
		bool skipDependencies);
	void recognizeCollected();
	void trackSkipLanguages();
	void checkRecognized();
	void checkRecognized(const std::vector<LanguageId> &skip);
	void applyLimit();
	void requestSome();
	void cancelToRequest();
	void cancelSentRequest();
	void switchTranslation(not_null<HistoryItem*> item, LanguageId id);

	void requestDone(
		LanguageId to,
		const QVector<MTPTextWithEntities> &list);

	const not_null<History*> _history;
	rpl::variable<bool> _trackingLanguage = false;
	base::flat_map<FullMsgId, ItemForRecognize> _itemsForRecognize;
	uint64 _generation = 0;
	int _limit = 0;
	int _addedInBunch = -1;
	bool _allLoaded = false;

	base::flat_map<not_null<HistoryItem*>, LanguageId> _switchTranslations;
	base::flat_map<FullMsgId, ItemToRequest> _itemsToRequest;
	std::vector<FullMsgId> _requested;
	mtpRequestId _requestId = 0;

	rpl::lifetime _trackingLifetime;
	rpl::lifetime _lifetime;

};

} // namespace HistoryView
