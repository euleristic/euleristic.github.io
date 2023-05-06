// C++20 or newer

#define EULERISTIC_JSON_COUT
#include "euleristic_json.hpp"

using namespace euleristic;

auto main(int argc, char* argv[]) -> int {
	if (argc != 3) {

		std::cout << "Parameter list incorrectly formatted, excepted: {executable} {target file path} {layout file path}.\n"
			<< "These may be absolute or relative to the current directory.\n";
		return -1;
	}

	std::filesystem::path target_path = argv[1];
	std::filesystem::path layout_path = argv[2];

	if (!target_path.is_absolute()) {
		target_path = std::filesystem::current_path() / target_path;
	};

	if (!layout_path.is_absolute()) {
		layout_path = std::filesystem::current_path() / layout_path;
	};

	json::value_type layout;
	try {
		layout = json::parse_file(layout_path);
	} catch (...) {
		std::cout << "Could not parse layout, no write to target made.\n";
		return -1;
	}

	std::string document;

	for (const auto& obj : layout.as_array()) {
		std::filesystem::path template_path = obj["template"].as_string();
		if (!template_path.is_absolute()) {
			template_path = layout_path.parent_path() / template_path;
		}
		if (!std::filesystem::exists(template_path)) {
			std::cout << "Layout specified non-existent path for template: " << obj["name"] << '\n';
			return -1;
		}
		std::ifstream template_document(template_path);
		if (!template_document.is_open()) {
			std::cout << "Could not open template document file specified by: " << obj["name"] << '\n';
			return -1;
		}
		std::stringstream buffer;
		buffer << template_document.rdbuf();

		if (obj["specializer"]) {
			std::filesystem::path specializer_path(obj["specializer"].as_string());
			if (!specializer_path.is_absolute()) {
				specializer_path = layout_path.parent_path() / specializer_path;
			}
			json::value_type specializer;
			try {
				specializer = json::parse_file(layout_path.parent_path() / specializer_path);
			} catch (...) {
				std::cout << "Could not parse specializer, no changes made.\n";
				return -1;
			}

			for (auto specialization : specializer.as_array()) {
				auto view = buffer.view();
				std::unordered_map<std::string, std::pair<std::string_view::iterator, std::string_view::iterator>> next_occurrences;

				while (view.size() > 0) {
					for (auto& key : specialization.as_object() | std::views::keys) {
						auto substr = std::ranges::search(view, std::string_view(key));
						next_occurrences[key] = std::make_pair(substr.begin(), substr.end());
					}
					auto next_key = std::ranges::min(next_occurrences | std::views::keys, [&next_occurrences](const auto& lhs, const auto& rhs) -> bool {
						return next_occurrences[lhs].first < next_occurrences[rhs].first;
					});
					auto [begin, end] = next_occurrences[next_key];
					document.append(view.begin(), begin);
					view = std::string_view(end, view.end());
					if (begin != end) {
						document.append(specialization[next_key].as_string());
					}
				}
			}
		}
		else {
			document.append(buffer.view());
		}
	}

	std::ofstream index(target_path);
	if (!index.is_open()) {
		std::cout << "Could not open target file, no changes made\n";
		return -1;
	}
	index << document;
	return 0;
}