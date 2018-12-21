namespace RTOSScheduling
{
    class ReadyTask
    {
        public Task Task { get; set; }

        public int CurrentCapacity { get; set; }

        public int CurrentDeadline { get; set; }
    }
}
