BEGIN{
	start=0;
	end=0;
        u=0;
}
{
        if (match($0, "Uncorrected Latency")) {
                u = 1;
        }
	if (u > 0 && start == 0) {
		if ($1 == "Value") {
			start = 1;
		}
	}

	if (u > 0 && $1 == "#[Mean") {
		end = 1;
	}

	if (u > 0 && start > 0 && end == 0) {
		print $1, "\t", $2;
	}
}
END{
}
